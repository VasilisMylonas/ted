#include "Editor.hpp"

#include <wx/event.h>
#include <wx/fdrepdlg.h>
#include <wx/notebook.h>
#include <wx/stc/stc.h>

Editor::Editor(wxWindow *parent) : wxPanel(parent) {
  auto sizer = new wxBoxSizer(wxVERTICAL);
  textCtrl = new wxStyledTextCtrl(this, wxID_ANY);
  sizer->Add(textCtrl, 1, wxEXPAND);
  SetSizerAndFit(sizer);

  textCtrl->Bind(wxEVT_STC_UPDATEUI, &Editor::OnCaretPositionChanged, this);
}

Editor::Editor(wxWindow *parent, const std::string &path) : Editor(parent) {
  Load(path);
}

void Editor::Close() {
  if (!textCtrl->GetModify()) {
    return;
  }

  if (ShowUnsavedChangesDialog()) {
    Save();
  }
}

void Editor::Load(const std::string &path) {
  this->path = path;
  textCtrl->LoadFile(path);
}

void Editor::Save() {
  if (path.empty()) {
    auto newPath = ShowSaveFileDialog();
    if (!newPath) {
      return;
    }

    path = newPath.value();
  }

  textCtrl->SaveFile(path);
}

void Editor::SaveAs() {
  auto newPath = ShowSaveFileDialog();
  if (!newPath) {
    return;
  }

  path = newPath.value();
  textCtrl->SaveFile(path);
}

bool Editor::IsModified() { return textCtrl->GetModify(); }

std::string Editor::GetTitle() {
  if (path.empty()) {
    return "Untitled";
  }

  auto pos = path.find_last_of("/\\");
  if (pos == std::string::npos) {
    return path;
  }

  return path.substr(pos + 1);
}

bool Editor::ShowUnsavedChangesDialog() {
  return unsavedChangesDialog.ShowModal() == wxID_YES;
}

std::optional<std::string> Editor::ShowSaveFileDialog() {
  if (saveFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }
  return saveFileDialog.GetPath().ToStdString();
}

void Editor::Paste() { textCtrl->Paste(); }
void Editor::Copy() { textCtrl->Copy(); }
void Editor::Cut() { textCtrl->Cut(); }
void Editor::Undo() { textCtrl->Undo(); }
void Editor::Redo() { textCtrl->Redo(); }

void Editor::Find() {
  if (findDialog) {
    findDialog->SetFocus();
    return;
  }

  findDialog = new wxFindReplaceDialog(this, &findReplaceData, wxT("Find"),
                                       wxFR_NOUPDOWN);
  findDialog->Bind(wxEVT_FIND, &Editor::OnFind, this);
  findDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindNext, this);
  findDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogClose, this);
  findDialog->Show();
}

void Editor::Replace() {
  if (replaceDialog) {
    replaceDialog->SetFocus();
    return;
  }

  replaceDialog =
      new wxFindReplaceDialog(this, &findReplaceData, wxT("Find and Replace"),
                              wxFR_REPLACEDIALOG | wxFR_NOUPDOWN);

  replaceDialog->Bind(wxEVT_FIND, &Editor::OnFind, this);
  replaceDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindNext, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE, &Editor::OnFindReplace, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE_ALL, &Editor::OnFindReplaceAll, this);
  replaceDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogClose, this);
  replaceDialog->Show();
}

void Editor::OnFindDialogClose(wxFindDialogEvent &event) {
  auto dialog = event.GetDialog();

  if (dialog == findDialog) {
    findDialog->Destroy();
    findDialog = nullptr;
  } else if (dialog == replaceDialog) {
    replaceDialog->Destroy();
    replaceDialog = nullptr;
  }
}

static int FindDialogEventFlagsToSearchFlags(int flags) {
  int searchFlags = 0;
  if (flags & wxFR_MATCHCASE) {
    searchFlags |= wxSTC_FIND_MATCHCASE;
  }
  if (flags & wxFR_WHOLEWORD) {
    searchFlags |= wxSTC_FIND_WHOLEWORD;
  }
  return searchFlags;
}

void Editor::DoFindReplace(int searchFlags, const std::string &findText,
                           bool next, bool replace,
                           const std::string &replaceText, bool replaceAll) {
  textCtrl->SetSearchFlags(searchFlags);

  if (replaceAll) {
    int count = 0;
    textCtrl->BeginUndoAction();

    // Start from the beginning of the document
    textCtrl->SetTargetStart(0);
    textCtrl->SetTargetEnd(textCtrl->GetTextLength());
    int pos = textCtrl->SearchInTarget(findText);

    while (pos >= 0) {
      textCtrl->SetTargetStart(pos);
      textCtrl->SetTargetEnd(pos + findText.length());
      textCtrl->ReplaceTarget(replaceText);
      count++;

      // Continue searching after the replaced text
      textCtrl->SetTargetStart(pos + replaceText.length());
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      pos = textCtrl->SearchInTarget(findText);
    }

    textCtrl->EndUndoAction();

    wxString message = wxString::Format(wxT("Replaced %d occurrences"), count);
    wxMessageBox(message, wxT("Replace All"), wxOK | wxICON_INFORMATION);
    return;
  }

  // Regular find/replace (not replace all)
  textCtrl->SetTargetStart(textCtrl->GetCurrentPos());

  if (next) {
    textCtrl->SetTargetStart(textCtrl->GetSelectionEnd());
  }

  textCtrl->SetTargetEnd(textCtrl->GetTextLength());
  int pos = textCtrl->SearchInTarget(findText);

  if (pos >= 0) {
    textCtrl->SetSelection(pos, pos + findText.length());
    if (replace) {
      textCtrl->ReplaceSelection(replaceText);
      textCtrl->SetSelection(pos, pos + replaceText.length());
    }
    textCtrl->EnsureCaretVisible();
    return;
  }

  // Not found, wrap search to the beginning
  textCtrl->SetTargetStart(0);
  textCtrl->SetTargetEnd(textCtrl->GetTextLength());
  pos = textCtrl->SearchInTarget(findText);

  if (pos >= 0) {
    textCtrl->SetSelection(pos, pos + findText.length());
    if (replace) {
      textCtrl->ReplaceSelection(replaceText);
      textCtrl->SetSelection(pos, pos + replaceText.length());
    }
    textCtrl->EnsureCaretVisible();
    wxMessageBox(wxT("Search wrapped to the beginning of the document"),
                 wxT("Find"), wxOK | wxICON_INFORMATION);
    return;
  }

  // Not found, text must not exist
  textCtrl->SetSelection(0, 0);
  wxMessageBox(wxT("Text not found"), wxT("Find"), wxOK | wxICON_INFORMATION);
}

void Editor::OnFind(wxFindDialogEvent &event) {
  findText = event.GetFindString();
  searchFlags = FindDialogEventFlagsToSearchFlags(event.GetFlags());
  DoFindReplace(searchFlags, findText.ToStdString());
}

void Editor::OnFindNext(wxFindDialogEvent &event) {
  findText = event.GetFindString();
  searchFlags = FindDialogEventFlagsToSearchFlags(event.GetFlags());
  DoFindReplace(searchFlags, findText.ToStdString(), true);
}

void Editor::OnFindReplace(wxFindDialogEvent &event) {
  findText = event.GetFindString();
  replaceText = event.GetReplaceString();
  searchFlags = FindDialogEventFlagsToSearchFlags(event.GetFlags());
  DoFindReplace(searchFlags, findText.ToStdString(), false, true,
                replaceText.ToStdString());
}

void Editor::OnFindReplaceAll(wxFindDialogEvent &event) {
  findText = event.GetFindString();
  replaceText = event.GetReplaceString();
  searchFlags = FindDialogEventFlagsToSearchFlags(event.GetFlags());
  DoFindReplace(searchFlags, findText.ToStdString(), false, true,
                replaceText.ToStdString(), true);
}

void Editor::OnCaretPositionChanged(wxStyledTextEvent &event) {
  int pos = textCtrl->GetCurrentPos();
  int line = textCtrl->LineFromPosition(pos);
  int col = textCtrl->GetColumn(pos);

  auto statusEvent = new wxCommandEvent(wxEVT_COMMAND_TEXT_UPDATED, GetId());

  statusEvent->SetInt(line + 1);
  statusEvent->SetExtraLong(col + 1);

  wxQueueEvent(GetParent(), statusEvent);

  event.Skip();
}

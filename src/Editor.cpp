#include "Editor.hpp"
#include <unordered_map>
#include <wx/event.h>
#include <wx/notebook.h>
#include <wx/stc/stc.h>

Editor::Editor(wxWindow *parent) : wxPanel(parent) {
  auto sizer = new wxBoxSizer(wxVERTICAL);
  textCtrl = new wxStyledTextCtrl(this, wxID_ANY);
  sizer->Add(textCtrl, 1, wxEXPAND);
  SetSizerAndFit(sizer);

  // Bind events
  textCtrl->Bind(wxEVT_CHAR, &Editor::OnChar, this);
  textCtrl->Bind(wxEVT_STC_UPDATEUI, &Editor::OnCaretPositionChanged, this);

  // Set up the editor with default settings and style
  SetupEditor();
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

  // Set syntax highlighting based on file extension
  SetSyntaxHighlighting(path);
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

// --- Find & Replace Dialogs ---
void Editor::Find() {
  if (findDialog) {
    findDialog->SetFocus();
    return;
  }
  findDialog = new wxFindReplaceDialog(this, &findReplaceData, wxT("Find"),
                                       wxFR_NOUPDOWN | wxFR_NOWHOLEWORD |
                                           wxFR_NOMATCHCASE);
  findDialog->Bind(wxEVT_FIND, &Editor::OnFindDialogEvents, this);
  findDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindDialogEvents, this);
  findDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogEvents, this);
  findDialog->Show();
}

void Editor::Replace() {
  if (replaceDialog) {
    replaceDialog->SetFocus();
    return;
  }
  replaceDialog = new wxFindReplaceDialog(
      this, &findReplaceData, wxT("Find and Replace"),
      wxFR_REPLACEDIALOG | wxFR_NOUPDOWN | wxFR_NOWHOLEWORD | wxFR_NOMATCHCASE);
  replaceDialog->Bind(wxEVT_FIND, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE_ALL, &Editor::OnFindDialogEvents,
                      this);
  replaceDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogEvents, this);
  replaceDialog->Show();
}

void Editor::OnFindDialogEvents(wxFindDialogEvent &event) {
  wxEventType type = event.GetEventType();
  if (type == wxEVT_FIND_CLOSE) {
    wxFindReplaceDialog *dlg = event.GetDialog();
    if (dlg == findDialog) {
      findDialog->Destroy();
      findDialog = nullptr;
    } else if (dlg == replaceDialog) {
      replaceDialog->Destroy();
      replaceDialog = nullptr;
    }
    return;
  }
  findText = event.GetFindString();
  int flags = event.GetFlags();
  searchFlags = 0;
  if (flags & wxFR_MATCHCASE)
    searchFlags |= wxSTC_FIND_MATCHCASE;
  if (flags & wxFR_WHOLEWORD)
    searchFlags |= wxSTC_FIND_WHOLEWORD;
  int currentPos = textCtrl->GetCurrentPos();
  int selectionStart = textCtrl->GetSelectionStart();
  int selectionEnd = textCtrl->GetSelectionEnd();
  int searchStartPos = (type == wxEVT_FIND)        ? currentPos
                       : (type == wxEVT_FIND_NEXT) ? selectionEnd
                                                   : selectionStart;
  if (type == wxEVT_FIND || type == wxEVT_FIND_NEXT) {
    textCtrl->SetSearchFlags(searchFlags);
    textCtrl->SetTargetStart(searchStartPos);
    textCtrl->SetTargetEnd(textCtrl->GetTextLength());
    int pos = textCtrl->SearchInTarget(findText);
    if (pos >= 0) {
      textCtrl->SetSelection(pos, pos + findText.length());
      textCtrl->EnsureCaretVisible();
    } else if (searchStartPos > 0) {
      textCtrl->SetTargetStart(0);
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      pos = textCtrl->SearchInTarget(findText);
      if (pos >= 0) {
        textCtrl->SetSelection(pos, pos + findText.length());
        textCtrl->EnsureCaretVisible();
        wxMessageBox(wxT("Search wrapped to the beginning of the document"),
                     wxT("Find"), wxOK | wxICON_INFORMATION);
      } else {
        wxMessageBox(wxT("Text not found"), wxT("Find"),
                     wxOK | wxICON_INFORMATION);
      }
    } else {
      wxMessageBox(wxT("Text not found"), wxT("Find"),
                   wxOK | wxICON_INFORMATION);
    }
  } else if (type == wxEVT_FIND_REPLACE) {
    wxString currentSelection = textCtrl->GetSelectedText();
    if (!currentSelection.IsEmpty() &&
        ((flags & wxFR_MATCHCASE && currentSelection == findText) ||
         (!(flags & wxFR_MATCHCASE) &&
          currentSelection.Lower() == findText.Lower()))) {
      replaceText = event.GetReplaceString();
      textCtrl->ReplaceSelection(replaceText);
      textCtrl->SetSelection(selectionStart,
                             selectionStart + replaceText.length());
      textCtrl->SetSearchFlags(searchFlags);
      textCtrl->SetTargetStart(selectionStart + replaceText.length());
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      int pos = textCtrl->SearchInTarget(findText);
      if (pos >= 0) {
        textCtrl->SetSelection(pos, pos + findText.length());
        textCtrl->EnsureCaretVisible();
      }
    } else {
      textCtrl->SetSearchFlags(searchFlags);
      textCtrl->SetTargetStart(currentPos);
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      int pos = textCtrl->SearchInTarget(findText);
      if (pos >= 0) {
        textCtrl->SetSelection(pos, pos + findText.length());
        textCtrl->EnsureCaretVisible();
      }
    }
  } else if (type == wxEVT_FIND_REPLACE_ALL) {
    int count = 0;
    replaceText = event.GetReplaceString();
    textCtrl->BeginUndoAction();
    textCtrl->SetSearchFlags(searchFlags);
    textCtrl->SetTargetStart(0);
    textCtrl->SetTargetEnd(textCtrl->GetTextLength());
    int pos = textCtrl->SearchInTarget(findText);
    while (pos >= 0) {
      textCtrl->SetTargetStart(pos);
      textCtrl->SetTargetEnd(pos + findText.length());
      textCtrl->ReplaceTarget(replaceText);
      count++;
      textCtrl->SetTargetStart(pos + replaceText.length());
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      pos = textCtrl->SearchInTarget(findText);
    }
    textCtrl->EndUndoAction();
    wxString message = wxString::Format(wxT("Replaced %d occurrences"), count);
    wxMessageBox(message, wxT("Replace All"), wxOK | wxICON_INFORMATION);
  }
}

// --- Editor Setup and Syntax Highlighting ---
void Editor::SetupEditor() {
  textCtrl->SetTabWidth(4);
  textCtrl->SetUseTabs(false);
  textCtrl->SetBackSpaceUnIndents(true);
  textCtrl->SetTabIndents(true);
  textCtrl->SetIndent(4);
  textCtrl->SetIndentationGuides(true);
  textCtrl->SetMarginWidth(0,
                           textCtrl->TextWidth(wxSTC_STYLE_LINENUMBER, "_999"));
  textCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
  textCtrl->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
  textCtrl->SetMarginWidth(1, 16);
  textCtrl->SetMarginMask(1, wxSTC_MASK_FOLDERS);
  textCtrl->SetMarginSensitive(1, true);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
  textCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
  textCtrl->SetEdgeColumn(80);
  textCtrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
  textCtrl->SetViewEOL(false);
  textCtrl->SetWrapMode(wxSTC_WRAP_WORD);
  textCtrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
  ApplyStyle(wxSTC_LEX_NULL);
}

void Editor::SetSyntaxHighlighting(const std::string &filename) {
  if (filename.empty()) {
    ApplyStyle(wxSTC_LEX_NULL);
    return;
  }
  size_t dotPos = filename.find_last_of('.');
  if (dotPos == std::string::npos) {
    ApplyStyle(wxSTC_LEX_NULL);
    return;
  }

  auto ext = filename.substr(dotPos);
  for (auto &c : ext) {
    c = tolower(c);
  }

  ApplyStyle(GetLexerFromFileExtension(ext));
}

void Editor::StyleClearAll() {
  for (int i = 0; i < wxSTC_STYLE_MAX; i++) {
    textCtrl->StyleSetForeground(i, *wxBLACK);
    textCtrl->StyleSetBackground(i, *wxWHITE);
    textCtrl->StyleSetFont(i, wxFont(10, wxFONTFAMILY_MODERN,
                                     wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->StyleSetBold(i, false);
    textCtrl->StyleSetItalic(i, false);
    textCtrl->StyleSetUnderline(i, false);
  }
}

void Editor::ApplyStyle(int lexer) {
  currentLexer = lexer;
  textCtrl->SetLexer(lexer);
  StyleClearAll();
  wxFont baseFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_NORMAL);
  textCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, baseFont);
  textCtrl->StyleClearAll();
  switch (lexer) {
  case wxSTC_LEX_CPP: {
    textCtrl->SetKeyWords(
        0,
        "auto break case char const continue default do double else enum "
        "extern float for goto if int long register return short signed sizeof "
        "static struct switch typedef union unsigned void volatile while class "
        "namespace template try catch throw new delete using inline virtual "
        "public protected private friend operator true false nullptr this");
    textCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 0));
    textCtrl->StyleSetItalic(wxSTC_C_COMMENT, true);
    textCtrl->StyleSetItalic(wxSTC_C_COMMENTLINE, true);
    textCtrl->StyleSetItalic(wxSTC_C_COMMENTDOC, true);
    textCtrl->StyleSetForeground(wxSTC_C_NUMBER, wxColour(128, 0, 128));
    textCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
    textCtrl->StyleSetBold(wxSTC_C_WORD, true);
    textCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 64, 0));
    textCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 0));
    textCtrl->StyleSetBold(wxSTC_C_OPERATOR, true);
    break;
  }
  case wxSTC_LEX_PYTHON: {
    textCtrl->SetKeyWords(
        0, "and as assert break class continue def del elif else except exec "
           "finally for from global if import in is lambda not or pass print "
           "raise return try while with yield");
    textCtrl->StyleSetForeground(wxSTC_P_COMMENTLINE, wxColour(0, 128, 0));
    textCtrl->StyleSetItalic(wxSTC_P_COMMENTLINE, true);
    textCtrl->StyleSetForeground(wxSTC_P_NUMBER, wxColour(128, 0, 128));
    textCtrl->StyleSetForeground(wxSTC_P_STRING, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_CHARACTER, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_TRIPLE, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_TRIPLEDOUBLE, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_WORD, wxColour(0, 0, 255));
    textCtrl->StyleSetBold(wxSTC_P_WORD, true);
    textCtrl->StyleSetForeground(wxSTC_P_CLASSNAME, wxColour(0, 64, 128));
    textCtrl->StyleSetBold(wxSTC_P_CLASSNAME, true);
    textCtrl->StyleSetForeground(wxSTC_P_DEFNAME, wxColour(0, 128, 128));
    textCtrl->StyleSetBold(wxSTC_P_DEFNAME, true);
    textCtrl->StyleSetForeground(wxSTC_P_OPERATOR, wxColour(0, 0, 0));
    textCtrl->StyleSetBold(wxSTC_P_OPERATOR, true);
    break;
  }
  case wxSTC_LEX_HTML:
  case wxSTC_LEX_XML: {
    textCtrl->SetKeyWords(0, "html body head title div span p h1 h2 h3 h4 h5 "
                             "h6 ul ol li a img table tr td th");
    textCtrl->StyleSetForeground(wxSTC_H_TAG, wxColour(0, 0, 128));
    textCtrl->StyleSetBold(wxSTC_H_TAG, true);
    textCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTE, wxColour(0, 0, 255));
    textCtrl->StyleSetForeground(wxSTC_H_DOUBLESTRING, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_H_SINGLESTRING, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_H_COMMENT, wxColour(0, 128, 0));
    textCtrl->StyleSetItalic(wxSTC_H_COMMENT, true);
    break;
  }
  default:
    break;
  }
}

// --- Caret and Status ---
void Editor::OnCaretPositionChanged(wxStyledTextEvent &event) {
  int pos = textCtrl->GetCurrentPos();
  int line = textCtrl->LineFromPosition(pos);
  int col = textCtrl->GetColumn(pos);
  wxCommandEvent statusEvent(wxEVT_COMMAND_TEXT_UPDATED, GetId());
  statusEvent.SetInt(line + 1);
  statusEvent.SetExtraLong(col + 1);
  statusEvent.SetString(GetLanguageNameFromLexer(currentLexer));
  wxPostEvent(GetParent(), statusEvent);
  event.Skip();
}

static std::unordered_map<int, std::string> languageMap = {
    {wxSTC_LEX_CPP, "C++"},
    {wxSTC_LEX_PYTHON, "Python"},
    {wxSTC_LEX_NULL, "Plain Text"},
};

static std::unordered_map<std::string, int> extensionMap = {
    {".cpp", wxSTC_LEX_CPP},  {".cc", wxSTC_LEX_CPP},
    {".cxx", wxSTC_LEX_CPP},  {".h", wxSTC_LEX_CPP},
    {".c", wxSTC_LEX_CPP},    {".hpp", wxSTC_LEX_CPP},
    {".hxx", wxSTC_LEX_CPP},  {".py", wxSTC_LEX_PYTHON},
    {".txt", wxSTC_LEX_NULL}, {".text", wxSTC_LEX_NULL},
};

int Editor::GetLexerFromFileExtension(const std::string &fileExtension) {
  return extensionMap.contains(fileExtension) ? extensionMap[fileExtension]
                                              : wxSTC_LEX_NULL;
}

std::string Editor::GetLanguageNameFromLexer(int lexer) {
  return languageMap.contains(lexer) ? languageMap.at(lexer) : "Unknown";
}

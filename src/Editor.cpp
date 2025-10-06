#include "Editor.hpp"
#include <wx/event.h>
#include <wx/notebook.h>

Editor::Editor(wxWindow *parent) : wxPanel(parent) {
  auto sizer = new wxBoxSizer(wxVERTICAL);
  textCtrl = new wxStyledTextCtrl(this, wxID_ANY);
  sizer->Add(textCtrl, 1, wxEXPAND);
  SetSizerAndFit(sizer);
  textCtrl->Bind(wxEVT_CHAR, &Editor::OnChar, this);
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
  if (unsavedChangesDialog.ShowModal() == wxID_YES) {
    return true;
  }
  return false;
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
  // TODO
}
void Editor::Replace() {
  // TODO
}

void Editor::OnChar(wxKeyEvent &event) {
  if (event.GetKeyCode() == WXK_SPACE) {
    textCtrl->BeginUndoAction();
    event.Skip();
    textCtrl->EndUndoAction();
  } else {
    event.Skip();
  }
}

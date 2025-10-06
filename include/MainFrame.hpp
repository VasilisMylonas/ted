#pragma once

#include <optional>
#include <string>
#include <vector>

#include <wx/notebook.h>
#include <wx/wx.h>

#include "Editor.hpp"

class MainFrame : public wxFrame {
public:
  MainFrame();

private:
  wxMenuBar *CreateMenuBar();
  void CreateFileMenu();
  void CreateEditMenu();

  std::optional<std::string> ShowOpenFileDialog();
  void SelectionChanged();
  void AddEditor(Editor *editor);

  void OnFileNew(wxCommandEvent &event);
  void OnFileOpen(wxCommandEvent &event);
  void OnFileClose(wxCommandEvent &event);
  void OnFileCloseAll(wxCommandEvent &event);
  void OnFileSave(wxCommandEvent &event);
  void OnFileSaveAs(wxCommandEvent &event);
  void OnFileQuit(wxCommandEvent &event);

  void OnEditUndo(wxCommandEvent &event);
  void OnEditRedo(wxCommandEvent &event);
  void OnEditCut(wxCommandEvent &event);
  void OnEditCopy(wxCommandEvent &event);
  void OnEditPaste(wxCommandEvent &event);
  void OnEditFind(wxCommandEvent &event);
  void OnEditReplace(wxCommandEvent &event);

  void OnSelectionChanged(wxNotebookEvent &event);
  void OnEditorChanged(wxStyledTextEvent &event);
  void OnClose(wxCloseEvent &event);

  wxMenu *fileMenu;
  wxMenu *editMenu;
  wxNotebook *notebook;
  std::vector<Editor *> editors;

  wxFileDialog openFileDialog{
      this,          wxT("Open File"),      wxEmptyString,
      wxEmptyString, wxT("Any File (*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST,
  };

  wxDECLARE_EVENT_TABLE();
};

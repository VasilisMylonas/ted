#pragma once

#include "TextDocument.hpp"
#include <vector>
#include <wx/config.h>
#include <wx/filehistory.h>
#include <wx/notebook.h>
#include <wx/wx.h>

#include <optional>
#include <string>

class MainFrame : public wxFrame {
public:
  MainFrame();

  // void AddDocument(const std::string &path = "");
  // void RemoveSelected();
  //   bool closeDocument(DocumentPanel *document);
  //   wxFileHistory history;

private:
  wxMenuBar *CreateMenuBar();
  void CreateFileMenu();
  void CreateEditMenu();
  void EnableMenus(bool enable);
  std::optional<std::string> ShowSaveFileDialog();
  std::optional<std::string> ShowOpenFileDialog();
  std::optional<bool> ShowUnsavedChangesDialog();

  void OnFileNew(wxCommandEvent &event);
  void OnFileOpen(wxCommandEvent &event);
  void OnFileClose(wxCommandEvent &event);
  void OnFileSave(wxCommandEvent &event);
  void OnFileSaveAs(wxCommandEvent &event);
  void OnFileQuit(wxCommandEvent &event);
  void OnSelectionChange(wxNotebookEvent &event);
  void OnClose(wxCloseEvent &event);

  wxMenu *fileMenu;
  wxMenu *editMenu;
  wxNotebook *notebook;
  std::vector<TextDocument> documents;

  std::optional<std::reference_wrapper<TextDocument>> GetSelectedDocument();

  wxMessageDialog unsavedChangesDialog{
      this,
      wxT("You have unsaved changes. Your changes will be lost if you don't "
          "save."),
      wxT("Unsaved Changes"),
      wxYES_NO | wxCANCEL | wxICON_WARNING,
  };

  wxFileDialog saveFileDialog{
      this,
      wxT("Save File"),
      wxEmptyString,
      wxEmptyString,
      wxT("Text Files (*.txt)|*.txt"),
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
  };

  wxFileDialog openFileDialog{
      this,
      wxT("Open File"),
      wxEmptyString,
      wxEmptyString,
      wxT("Text Files (*.txt)|*.txt"),
      wxFD_OPEN | wxFD_FILE_MUST_EXIST,
  };

  wxDECLARE_EVENT_TABLE();
};

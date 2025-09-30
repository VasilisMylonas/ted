#include "MainFrame.hpp"
#include "TextDocument.hpp"
#include <functional>
#include <optional>
#include <wx/stc/stc.h>

enum {
  ID_NOTEBOOK = 10'000,
};

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_NEW, MainFrame::OnFileNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnFileOpen)
    EVT_MENU(wxID_CLOSE, MainFrame::OnFileClose)
    EVT_MENU(wxID_EXIT, MainFrame::OnFileQuit)
    EVT_MENU(wxID_SAVE, MainFrame::OnFileSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnFileSaveAs)
    EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE();
// clang-format on

void MainFrame::CreateFileMenu() {
  fileMenu = new wxMenu();
  fileMenu->Append(wxID_NEW);
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_OPEN);
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_SAVE);
  fileMenu->Append(wxID_SAVEAS);
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_CLOSE);
  // Problematic: wxWidgets bug?
  // fileMenu->Append(wxID_CLOSE_ALL);
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT);

  //   history.UseMenu(fileMenu);
}

void MainFrame::CreateEditMenu() {
  editMenu = new wxMenu();
  editMenu->Append(wxID_UNDO);
  editMenu->Append(wxID_REDO);
  editMenu->AppendSeparator();
  editMenu->Append(wxID_COPY);
  editMenu->Append(wxID_CUT);
  editMenu->Append(wxID_PASTE);
  editMenu->AppendSeparator();
  editMenu->Append(wxID_FIND);
  editMenu->Append(wxID_REPLACE);
}

wxMenuBar *MainFrame::CreateMenuBar() {
  CreateFileMenu();
  CreateEditMenu();

  auto menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, wxT("&File"));
  menuBar->Append(editMenu, wxT("&Edit"));

  return menuBar;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxT("Ted")) {

  notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxNB_MULTILINE);

  auto sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(notebook, 1, wxEXPAND);

  notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnSelectionChange,
                 this);

  SetMenuBar(CreateMenuBar());
  SetSizerAndFit(sizer);
  SetMinClientSize(wxSize(400, 300));

  //   unsavedChangesDialog.SetYesNoCancelLabels(wxT("Save"), wxT("Don't save"),
  //                                             wxT("Cancel"));

  EnableMenus(false);

  //   history.Load(*wxConfig::Get());
}

void MainFrame::EnableMenus(bool enable) {
  fileMenu->Enable(wxID_CLOSE, enable);
  fileMenu->Enable(wxID_SAVE, enable);
  fileMenu->Enable(wxID_SAVEAS, enable);
}

void MainFrame::OnClose([[maybe_unused]] wxCloseEvent &event) {
  //   DocumentPanel *selected = nullptr;
  //   while ((selected = getSelected())) {
  //     if (!closeDocument(selected)) {
  //       return;
  //     }

  //     notebook->RemovePage(notebook->GetSelection());

  //     if (notebook->GetPageCount() == 0) {
  //       enableMenus(false);
  //     }
  //   }

  //   history.Save(*wxConfig::Get());
  event.Skip();
}

void MainFrame::OnFileQuit([[maybe_unused]] wxCommandEvent &event) { Close(); }

void MainFrame::OnFileSave([[maybe_unused]] wxCommandEvent &event) {
  //   auto selected = getSelected();
  //   if (!selected) {
  //     return;
  //   }

  //   if (selected->Path().empty()) {
  //     auto path = showSaveDialog();
  //     if (!path) {
  //       return;
  //     }

  //     selected->SetPath(*path);
  //     notebook->SetPageText(notebook->GetSelection(), selected->Title());
  //   }

  //   selected->Save();
}

void MainFrame::OnFileSaveAs([[maybe_unused]] wxCommandEvent &event) {
  auto selected = GetSelectedDocument();
  if (!selected) {
    return;
  }

  auto path = ShowSaveFileDialog();
  if (!path) {
    return;
  }

  //   selected->Discard();

  //   auto document = new DocumentPanel(notebook, path.value());
  //   notebook->AddPage(document, document->Title(), true);
  //   document->Save();
}

void MainFrame::OnFileOpen([[maybe_unused]] wxCommandEvent &event) {
  auto path = ShowOpenFileDialog();
  if (!path) {
    return;
  }

  auto &document = documents.emplace_back(path.value());

  auto editor = new wxStyledTextCtrl(notebook, wxID_ANY);

  editor->SetText(document.GetContent());
  editor->SetLexer(wxSTC_LEX_NULL);
  notebook->AddPage(editor, document.GetPath(), true);

  //   history.AddFileToHistory(path.value());
  // auto document = new DocumentPanel(notebook, path.value());
  //   enableMenus(true);
}

void MainFrame::OnFileNew([[maybe_unused]] wxCommandEvent &event) {

  //   auto document = new DocumentPanel(notebook);
  //   notebook->AddPage(document, document->Title(), true);
  //   enableMenus(true);
}

// bool MainFrame::closeDocument(DocumentPanel *document) {
//   if (!document->IsModified()) {
//     return true;
//   }

//   auto result = ShowUnsavedChangesDialog();

//   // Cancel
//   if (!result) {
//     return false;
//   }

//   // Don't save
//   if (!result.value()) {
//     return true;
//   }

//   auto path = showSaveDialog();
//   if (!path) {
//     return false;
//   }

//   document->SetPath(*path);
//   document->Save();
//   return true;
// }

void MainFrame::OnFileClose([[maybe_unused]] wxCommandEvent &event) {
  //   auto selection = notebook->GetSelection();

  //   if (!closeDocument(getSelected())) {
  //     return;
  //   }

  //   notebook->RemovePage(selection);

  //   if (notebook->GetPageCount() == 0) {
  //     enableMenus(false);
  //   }
}

void MainFrame::OnSelectionChange([[maybe_unused]] wxNotebookEvent &event) {
  //   auto selection = event.GetSelection();
  //   enableMenus(selection != wxNOT_FOUND);
  //   event.Skip();
}

std::optional<std::reference_wrapper<TextDocument>>
MainFrame::GetSelectedDocument() {
  if (notebook->GetPageCount() == 0) {
    return std::nullopt;
  }

  auto index = notebook->GetSelection();
  return documents[index];
}

std::optional<bool> MainFrame::ShowUnsavedChangesDialog() {
  switch (unsavedChangesDialog.ShowModal()) {
  case wxID_YES:
    return true;
  case wxID_NO:
    return false;
  default:
    return std::nullopt;
  }
}

std::optional<std::string> MainFrame::ShowSaveFileDialog() {
  if (saveFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }

  return saveFileDialog.GetPath().ToStdString();
}

std::optional<std::string> MainFrame::ShowOpenFileDialog() {
  if (openFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }

  return openFileDialog.GetPath().ToStdString();
}

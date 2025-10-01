#include "MainFrame.hpp"
#include "Editor.hpp"
#include <vector>
#include <wx/log.h>

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
  // menuBar->Append(editMenu, wxT("&Edit"));

  return menuBar;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxT("Ted")) {

  notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxNB_MULTILINE);

  auto sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(notebook, 1, wxEXPAND);
  notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnSelectionChanged,
                 this);

  SetMenuBar(CreateMenuBar());
  SetSizerAndFit(sizer);
  SetMinClientSize(wxSize(400, 300));

  SelectionChanged();
}

void MainFrame::OnClose([[maybe_unused]] wxCloseEvent &event) {
  for (auto &editor : editors) {
    editor->Close();
  }
  editors.clear();
  event.Skip();
}

void MainFrame::OnFileQuit([[maybe_unused]] wxCommandEvent &event) { Close(); }

void MainFrame::OnFileOpen([[maybe_unused]] wxCommandEvent &event) {
  auto path = ShowOpenFileDialog();
  if (!path) {
    return;
  }

  AddEditor(new Editor(notebook, path.value()));
}

void MainFrame::OnFileNew([[maybe_unused]] wxCommandEvent &event) {
  AddEditor(new Editor(notebook));
}

void MainFrame::OnFileSave([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }

  editors[index]->Save();
  notebook->SetPageText(index, editors[index]->GetTitle());
}

void MainFrame::OnFileSaveAs([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }

  editors[index]->SaveAs();
  notebook->SetPageText(index, editors[index]->GetTitle());
}

void MainFrame::OnFileClose([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();

  if (index == wxNOT_FOUND) {
    return;
  }

  auto editor = editors[index];
  editor->Close();
  std::erase(editors, editor);

  notebook->DeletePage(index);
}

void MainFrame::OnSelectionChanged([[maybe_unused]] wxNotebookEvent &event) {
  SelectionChanged();
  event.Skip();
}

void MainFrame::OnEditorChanged([[maybe_unused]] wxStyledTextEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }

  notebook->SetPageText(index, editors[index]->GetTitle() + "*");
  event.Skip();
}

void MainFrame::AddEditor(Editor *editor) {
  editors.push_back(editor);
  editor->Bind(wxEVT_STC_CHANGE, &MainFrame::OnEditorChanged, this);
  notebook->AddPage(editor, editor->GetTitle(), true);
  SelectionChanged();
}

void MainFrame::SelectionChanged() {
  bool hasTab = notebook->GetPageCount() > 0;

  fileMenu->Enable(wxID_CLOSE, hasTab);
  fileMenu->Enable(wxID_SAVE, hasTab);
  fileMenu->Enable(wxID_SAVEAS, hasTab);

  // editMenu->Enable(wxID_UNDO, hasTab);
  // editMenu->Enable(wxID_REDO, hasTab);
  // editMenu->Enable(wxID_COPY, hasTab);
  // editMenu->Enable(wxID_CUT, hasTab);
  // editMenu->Enable(wxID_PASTE, hasTab);
  // editMenu->Enable(wxID_FIND, hasTab);
  // editMenu->Enable(wxID_REPLACE, hasTab);
}

std::optional<std::string> MainFrame::ShowOpenFileDialog() {
  if (openFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }

  return openFileDialog.GetPath().ToStdString();
}

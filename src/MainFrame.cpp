#include "MainFrame.hpp"
#include "Editor.hpp"
#include <vector>
#include <wx/notebook.h>

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_NEW, MainFrame::OnFileNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnFileOpen)
    EVT_MENU(wxID_CLOSE, MainFrame::OnFileClose)
    EVT_MENU(wxID_CLOSE_ALL, MainFrame::OnFileCloseAll)
    EVT_MENU(wxID_EXIT, MainFrame::OnFileQuit)
    EVT_MENU(wxID_SAVE, MainFrame::OnFileSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnFileSaveAs)
    EVT_MENU(wxID_UNDO, MainFrame::OnEditUndo)
    EVT_MENU(wxID_REDO, MainFrame::OnEditRedo)
    EVT_MENU(wxID_CUT, MainFrame::OnEditCut)
    EVT_MENU(wxID_COPY, MainFrame::OnEditCopy)
    EVT_MENU(wxID_PASTE, MainFrame::OnEditPaste)
    EVT_MENU(wxID_FIND, MainFrame::OnEditFind)
    EVT_MENU(wxID_REPLACE, MainFrame::OnEditReplace)
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
  fileMenu->Append(wxID_CLOSE_ALL, "Close All\tCtrl+Shift+W");
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
  menuBar->Append(editMenu, wxT("&Edit"));

  return menuBar;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, wxT("Ted")) {

  notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxNB_MULTILINE);

  auto sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(notebook, 1, wxEXPAND);
  notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnSelectionChanged,
                 this);

  // Bind to the custom status update event
  Bind(wxEVT_COMMAND_TEXT_UPDATED, &MainFrame::OnEditorStatusUpdate, this);

  SetMenuBar(CreateMenuBar());
  SetSizerAndFit(sizer);
  SetMinClientSize(wxSize(400, 300));

  // Create status bar with three fields
  CreateStatusBar(3);
  int widths[] = {-1, 100, 150}; // -1 means variable width
  SetStatusWidths(3, widths);

  // Set initial status text
  SetStatusText(wxT("Ready"), 0);
  SetStatusText(wxT("Ln: 1, Col: 1"), 1);
  SetStatusText(wxT("Text"), 2);

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

void MainFrame::OnFileCloseAll([[maybe_unused]] wxCommandEvent &event) {
  // Make a copy of the editors vector since we'll be modifying it in the loop
  auto editorsCopy = editors;
  for (auto editor : editorsCopy) {
    editor->Close();
  }

  // Clear the editors vector and remove all pages from the notebook
  editors.clear();
  notebook->DeleteAllPages();

  // Update the menu items
  SelectionChanged();
}

void MainFrame::OnEditUndo([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Undo();
}

void MainFrame::OnEditRedo([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Redo();
}

void MainFrame::OnEditCut([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Cut();
}

void MainFrame::OnEditCopy([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Copy();
}

void MainFrame::OnEditPaste([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Paste();
}

void MainFrame::OnEditFind([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Find();
}

void MainFrame::OnEditReplace([[maybe_unused]] wxCommandEvent &event) {
  auto index = notebook->GetSelection();
  if (index == wxNOT_FOUND) {
    return;
  }
  editors[index]->Replace();
}

void MainFrame::OnSelectionChanged([[maybe_unused]] wxNotebookEvent &event) {
  SelectionChanged();

  // Update status bar with current editor information
  int index = notebook->GetSelection();
  if (index != wxNOT_FOUND && index < static_cast<int>(editors.size())) {
    SetStatusText(editors[index]->GetTitle(), 0);
  } else {
    SetStatusText(wxT("Ready"), 0);
    SetStatusText(wxT("Ln: 1, Col: 1"), 1);
    SetStatusText(wxT("Text"), 2);
  }

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

  // Update status bar with information from the newly added editor
  SetStatusText(editor->GetTitle(), 0);
}

void MainFrame::SelectionChanged() {
  bool hasTab = notebook->GetPageCount() > 0;

  fileMenu->Enable(wxID_CLOSE, hasTab);
  fileMenu->Enable(wxID_CLOSE_ALL, hasTab);
  fileMenu->Enable(wxID_SAVE, hasTab);
  fileMenu->Enable(wxID_SAVEAS, hasTab);
  editMenu->Enable(wxID_UNDO, hasTab);
  editMenu->Enable(wxID_REDO, hasTab);
  editMenu->Enable(wxID_COPY, hasTab);
  editMenu->Enable(wxID_CUT, hasTab);
  editMenu->Enable(wxID_PASTE, hasTab);
  editMenu->Enable(wxID_FIND, hasTab);
  editMenu->Enable(wxID_REPLACE, hasTab);
}

std::optional<std::string> MainFrame::ShowOpenFileDialog() {
  if (openFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }

  return openFileDialog.GetPath().ToStdString();
}

void MainFrame::OnEditorStatusUpdate(wxCommandEvent &event) {
  int line = event.GetInt();
  int col = event.GetExtraLong();
  auto language = event.GetString();

  // Update status bar with line/column information
  auto posText = wxString::Format(wxT("Ln: %d, Col: %d"), line, col);
  SetStatusText(posText, 1);

  // Update language information
  SetStatusText(language, 2);
}

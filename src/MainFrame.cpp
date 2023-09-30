#include "MainFrame.hpp"

enum
{
    ID_NOTEBOOK = 10'000,
    ID_INCREASE_FONT_SIZE,
    ID_DECREASE_FONT_SIZE,
};

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_NEW, MainFrame::onFileNew)
    EVT_MENU(wxID_OPEN, MainFrame::onFileOpen)
    EVT_MENU(wxID_CLOSE, MainFrame::onFileClose)
    EVT_MENU(wxID_SAVE, MainFrame::onFileSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::onFileSaveAs)
wxEND_EVENT_TABLE();
// clang-format on

wxMenuBar *MainFrame::createMenuBar()
{
    fileMenu = new wxMenu();
    fileMenu->Append(wxID_NEW);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_OPEN);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_SAVE);
    fileMenu->Append(wxID_SAVEAS);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CLOSE);
    // fileMenu->Append(wxID_CLOSE_ALL);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    history.UseMenu(fileMenu);

    auto edit = new wxMenu();
    edit->Append(wxID_UNDO);
    edit->Append(wxID_REDO);
    edit->AppendSeparator();
    edit->Append(wxID_COPY);
    edit->Append(wxID_CUT);
    edit->Append(wxID_PASTE);
    edit->AppendSeparator();
    edit->Append(wxID_FIND);
    edit->Append(wxID_REPLACE);

    auto view = new wxMenu();
    view->Append(ID_INCREASE_FONT_SIZE, "Font Size +");
    view->Append(ID_DECREASE_FONT_SIZE, "Font Size -");

    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(edit, wxT("&Edit"));
    menuBar->Append(view, wxT("&View"));

    return menuBar;
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, wxT("Ted"))
{
    unsavedChangesDialog.SetYesNoCancelLabels(wxT("Save"), wxT("Don't save"), wxT("Cancel"));

    SetMenuBar(createMenuBar());

    panel = new wxPanel(this, wxID_ANY);
    notebook = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE);

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(notebook, 1, wxEXPAND);

    notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::onSelectionChange, this);

    panel->SetSizerAndFit(sizer);

    enableMenus(false);
}

void MainFrame::enableMenus(bool enable)
{
    fileMenu->Enable(wxID_CLOSE, enable);
    fileMenu->Enable(wxID_SAVE, enable);
    fileMenu->Enable(wxID_SAVEAS, enable);
}

void MainFrame::onFileSave([[maybe_unused]] wxCommandEvent &event)
{
    auto selected = getSelected();
    if (!selected)
    {
        return;
    }

    selected->Save();
}

void MainFrame::onFileSaveAs([[maybe_unused]] wxCommandEvent &event)
{
    auto selected = getSelected();
    if (!selected)
    {
        return;
    }

    auto path = showSaveDialog();
    if (!path)
    {
        return;
    }

    // TODO
    selected->Reload();

    auto document = new Document(notebook, path.value());
    notebook->AddPage(document, document->Title(), true);
    document->Save();
}

void MainFrame::onFileOpen([[maybe_unused]] wxCommandEvent &event)
{
    auto path = showOpenDialog();
    if (!path)
    {
        return;
    }

    auto document = new Document(notebook, path.value());
    notebook->AddPage(document, document->Title(), true);
    enableMenus(true);
}

void MainFrame::onFileNew([[maybe_unused]] wxCommandEvent &event)
{
    auto document = new Document(notebook);
    notebook->AddPage(document, document->Title(), true);
    enableMenus(true);
}

void MainFrame::onFileClose([[maybe_unused]] wxCommandEvent &event)
{
    auto selection = notebook->GetSelection();
    // TODO: save
    notebook->RemovePage(selection);

    if (notebook->GetPageCount() == 0)
    {
        enableMenus(false);
    }
}

void MainFrame::onSelectionChange(wxNotebookEvent &event)
{
    auto selection = event.GetSelection();
    enableMenus(selection != wxNOT_FOUND);
    event.Skip();
}

Document *MainFrame::getSelected()
{
    if (notebook->GetPageCount() == 0)
    {
        return nullptr;
    }

    auto index = notebook->GetSelection();
    return static_cast<Document *>(notebook->GetPage(index));
}

std::optional<bool> MainFrame::ShowUnsavedChangesDialog()
{
    switch (unsavedChangesDialog.ShowModal())
    {
    case wxID_YES:
        return true;
    case wxID_NO:
        return false;
    default:
        return {};
    }
}

std::optional<std::string> MainFrame::showSaveDialog()
{
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return saveFileDialog.GetPath().ToStdString();
}

std::optional<std::string> MainFrame::showOpenDialog()
{
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return openFileDialog.GetPath().ToStdString();
}

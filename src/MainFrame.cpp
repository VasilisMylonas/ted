#include "MainFrame.hpp"

#include <iostream>
#include <fstream>

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, wxT("Ted")), presenter{*this}
{
    unsavedChangesDialog.SetYesNoCancelLabels(wxT("Save"), wxT("Don't save"), wxT("Cancel"));

    textArea = new wxTextCtrl{
        this,
        10001,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_WORDWRAP | wxTE_MULTILINE,
    };

    wxMenu *file = new wxMenu();
    file->Append(wxID_OPEN);
    file->AppendSeparator();
    file->Append(wxID_SAVE);
    file->Append(wxID_SAVEAS);
    file->AppendSeparator();
    file->Append(wxID_EXIT);

    history.UseMenu(file);

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(file, "&File");

    SetMenuBar(menuBar);

    // Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnQuit, &presenter, wxID_EXIT);
    // Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnOpen, &presenter, wxID_OPEN);
    // Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnSave, &presenter, wxID_SAVE);
    // Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnSaveAs, &presenter, wxID_SAVEAS);
    // Bind(wxEVT_CLOSE_WINDOW, &MainPresenter::OnClose, &presenter);

    presenter.Init();
}

void MainFrame::AddFileToHistory(const std::string &path)
{
    history.AddFileToHistory(path);
}

void MainFrame::SaveHistory(wxConfigBase &config)
{
    history.Save(config);
}

void MainFrame::LoadHistory(wxConfigBase &config)
{
    history.Load(config);
}

bool MainFrame::IsTextModified()
{
    return textArea->IsModified();
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

std::optional<std::string> MainFrame::ShowSaveDialog()
{
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return saveFileDialog.GetPath().ToStdString();
}

std::optional<std::string> MainFrame::ShowOpenDialog()
{
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return openFileDialog.GetPath().ToStdString();
}

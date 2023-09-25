#include "MainFrame.hpp"

#include "MenuBar.hpp"

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

    SetMenuBar(new MenuBar());

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnQuit, &presenter, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnOpen, &presenter, wxID_OPEN);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnSave, &presenter, wxID_SAVE);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainPresenter::OnSaveAs, &presenter, wxID_SAVEAS);
    Bind(wxEVT_CLOSE_WINDOW, &MainPresenter::OnClose, &presenter);
}

bool MainFrame::IsTextModified() const
{
    return textArea->IsModified();
}

std::optional<bool> MainFrame::ShowUnsavedChangesDialog() const
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

std::optional<std::string> MainFrame::ShowSaveDialog() const
{
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return saveFileDialog.GetPath().ToStdString();
}

std::optional<std::string> MainFrame::ShowOpenDialog() const
{
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return {};
    }

    return openFileDialog.GetPath().ToStdString();
}

#include "MainFrame.hpp"

#include "MenuBar.hpp"

#include <iostream>
#include <fstream>

/* clang-format off */
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveAs)
    EVT_CLOSE(MainFrame::OnClose)
wxEND_EVENT_TABLE();
/* clang-format on */

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, wxT("Ted"))
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
}

void MainFrame::SetCurrentFile(wxString path)
{
    currentFile = path;

    if (currentFile.IsEmpty())
    {
        SetTitle("Ted");
        textArea->Clear();
    }
    else
    {
        SetTitle(currentFile + " - Ted");
    }
}

void MainFrame::Open()
{
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    SetCurrentFile(openFileDialog.GetPath());
    Load();
}

void MainFrame::Load()
{
    textArea->LoadFile(currentFile);
}

void MainFrame::Save()
{
    textArea->SaveFile(currentFile);
}

bool MainFrame::SaveAs()
{
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return false;
    }

    SetCurrentFile(saveFileDialog.GetPath());
    Save();
    return true;
}

void MainFrame::OnNewFile([[maybe_unused]] wxCommandEvent &event)
{
    // TODO
}

void MainFrame::OnOpen([[maybe_unused]] wxCommandEvent &event)
{
    Open();
}

void MainFrame::OnSave([[maybe_unused]] wxCommandEvent &event)
{
    Save();
}

void MainFrame::OnSaveAs([[maybe_unused]] wxCommandEvent &event)
{
    SaveAs();
}

void MainFrame::OnClose([[maybe_unused]] wxCloseEvent &event)
{
    if (!textArea->IsModified())
    {
        event.Skip();
        return;
    }

    switch (unsavedChangesDialog.ShowModal())
    {
    case wxID_YES:
        if (currentFile.IsEmpty())
        {
            if (!SaveAs())
            {
                return;
            }
        }
        else
        {
            Save();
        }
        event.Skip();
        break;
    case wxID_NO:
        event.Skip();
        break;
    default:
        break;
    }
}

void MainFrame::OnQuit([[maybe_unused]] wxCommandEvent &event)
{
    Close();
}
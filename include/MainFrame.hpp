#pragma once

#include "Core.hpp"
#include "MainPresenter.hpp"

class MainFrame : public wxFrame
{
public:
    MainFrame();

    void OnNewFile(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnSaveAs(wxCommandEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);

private:
    wxDECLARE_EVENT_TABLE();

public:
    wxTextCtrl *textArea;

    wxMessageDialog unsavedChangesDialog{
        this,
        wxT("You have unsaved changes. Your changes will be lost if you don't save."),
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

    MainPresenter presenter;
};
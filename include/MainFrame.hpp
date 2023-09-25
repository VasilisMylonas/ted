#pragma once

#include "Core.hpp"
#include "MainPresenter.hpp"

#include <optional>
#include <string>

class MainFrame : public wxFrame
{
public:
    MainFrame();

    bool IsTextModified();
    std::optional<std::string> ShowSaveDialog();
    std::optional<std::string> ShowOpenDialog();
    std::optional<bool> ShowUnsavedChangesDialog();
    void AddFileToHistory(const std::string &path);
    void SaveHistory(wxConfigBase &config);
    void LoadHistory(wxConfigBase &config);

    wxTextCtrl *textArea;

private:
    MainPresenter presenter;
    wxFileHistory history;

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
};
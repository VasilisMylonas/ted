#pragma once

#include "Core.hpp"
#include "MainPresenter.hpp"

#include <optional>
#include <string>

class MainFrame : public wxFrame
{
public:
    MainFrame();

    bool IsTextModified() const;
    std::optional<std::string> ShowSaveDialog() const;
    std::optional<std::string> ShowOpenDialog() const;
    std::optional<bool> ShowUnsavedChangesDialog() const;

    wxTextCtrl *textArea;

private:
    mutable wxMessageDialog unsavedChangesDialog{
        this,
        wxT("You have unsaved changes. Your changes will be lost if you don't save."),
        wxT("Unsaved Changes"),
        wxYES_NO | wxCANCEL | wxICON_WARNING,
    };

    mutable wxFileDialog saveFileDialog{
        this,
        wxT("Save File"),
        wxEmptyString,
        wxEmptyString,
        wxT("Text Files (*.txt)|*.txt"),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
    };

    mutable wxFileDialog openFileDialog{
        this,
        wxT("Open File"),
        wxEmptyString,
        wxEmptyString,
        wxT("Text Files (*.txt)|*.txt"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST,
    };

    MainPresenter presenter;
};
#pragma once

#include "Core.hpp"
#include "Document.hpp"

#include <optional>
#include <string>

class MainFrame : public wxFrame
{
public:
    MainFrame();

    std::optional<std::string> showSaveDialog();
    std::optional<bool> ShowUnsavedChangesDialog();

    // void AddDocument(const std::string &path = "");
    // void RemoveSelected();
    // std::optional<Document *> GetSelected();

private:
    Document *getSelected();

    std::optional<std::string> showOpenDialog();

    void onFileNew(wxCommandEvent &event);
    void onFileOpen(wxCommandEvent &event);
    void onFileClose(wxCommandEvent &event);
    void onFileSave(wxCommandEvent &event);
    void onFileSaveAs(wxCommandEvent &event);
    void onFileQuit(wxCommandEvent &event);
    void onSelectionChange(wxNotebookEvent &event);
    void onClose(wxCloseEvent &event);

    bool closeDocument(Document *document);

    void enableMenus(bool enable);
    wxMenuBar *createMenuBar();

    wxMenu *fileMenu;
    wxPanel *panel;
    wxFileHistory history;
    wxNotebook *notebook;

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

    wxDECLARE_EVENT_TABLE();
};

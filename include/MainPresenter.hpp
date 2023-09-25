#pragma once

#include "Core.hpp"
#include "Presenter.hpp"

class MainFrame;

class MainPresenter : public Presenter<MainFrame>, public wxEvtHandler
{
public:
    MainPresenter(MainFrame &view);

    void Init();

    void OnNewFile(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnSaveAs(wxCommandEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);

private:
    void Quit();
    void Open();
    bool SaveAs();
    void Save();
    void SetCurrentFile(const std::string &path);
    bool SaveUnsavedChanges();

    std::string currentFile;
};

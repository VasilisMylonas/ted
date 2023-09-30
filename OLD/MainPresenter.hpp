#pragma once

#include "Core.hpp"
#include "Presenter.hpp"

class MainFrame;

class MainPresenter : public Presenter<MainFrame>, public wxEvtHandler
{
public:
    MainPresenter(MainFrame &view);

    void Init();

    void OnNew(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnSaveAs(wxCommandEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnClose(wxCommandEvent &event);
    void OnCloseWindow(wxCloseEvent &event);

private:
    void closeDocument(DocumentView *documentView);

    void quit();
    bool saveAs();
    void save();
};

#pragma once

#include "Core.hpp"
#include "Presenter.hpp"

class MainFrame;

class MainPresenter : public Presenter<MainFrame>
{
public:
    MainPresenter(MainFrame &view) : Presenter(view)
    {
    }

    void Open();
    bool SaveAs();
    void Save();
    void Load();
    void SetCurrentFile(const wxString &path);
    bool SaveOrSaveAs();

private:
    wxString currentFile{};
};

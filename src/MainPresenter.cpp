#include "MainPresenter.hpp"

#include "MainFrame.hpp"

MainPresenter::MainPresenter(MainFrame &view)
    : Presenter(view)
{
}

void MainPresenter::Open()
{
    auto path = View().ShowOpenDialog();

    if (!path)
    {
        return;
    }

    SetCurrentFile(*path);
    View().textArea->LoadFile(currentFile);
}

void MainPresenter::Save()
{
    View().textArea->SaveFile(currentFile);
}

bool MainPresenter::SaveAs()
{
    auto path = View().ShowSaveDialog();

    if (!path)
    {
        return false;
    }

    SetCurrentFile(*path);
    View().textArea->SaveFile(currentFile);
    return true;
}

bool MainPresenter::SaveUnsavedChanges()
{
    if (currentFile.IsEmpty())
    {
        return SaveAs();
    }

    Save();
    return true;
}

void MainPresenter::SetCurrentFile(const wxString &path)
{
    currentFile = path;

    if (currentFile.IsEmpty())
    {
        View().SetTitle("Ted");
        View().textArea->Clear();
    }
    else
    {
        View().SetTitle(currentFile + " - Ted");
    }
}

void MainPresenter::OnNewFile([[maybe_unused]] wxCommandEvent &event)
{
    // TODO
}

void MainPresenter::OnOpen([[maybe_unused]] wxCommandEvent &event)
{
    Open();
}

void MainPresenter::OnSave([[maybe_unused]] wxCommandEvent &event)
{
    Save();
}

void MainPresenter::OnSaveAs([[maybe_unused]] wxCommandEvent &event)
{
    SaveAs();
}

void MainPresenter::OnClose([[maybe_unused]] wxCloseEvent &event)
{
    if (!View().IsTextModified())
    {
        event.Skip();
        return;
    }

    auto result = View().ShowUnsavedChangesDialog();

    if (!result)
    {
        return;
    }

    if (*result && !SaveUnsavedChanges())
    {
        return;
    }

    event.Skip();
}

void MainPresenter::OnQuit([[maybe_unused]] wxCommandEvent &event)
{
    View().Close();
}

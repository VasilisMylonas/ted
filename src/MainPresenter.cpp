#include "MainPresenter.hpp"
#include "MainFrame.hpp"

void MainPresenter::Open()
{
    if (view().openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    SetCurrentFile(view().openFileDialog.GetPath());
    Load();
}

void MainPresenter::Load()
{
    view().textArea->LoadFile(currentFile);
}

void MainPresenter::Save()
{
    view().textArea->SaveFile(currentFile);
}

bool MainPresenter::SaveAs()
{
    if (view().saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return false;
    }

    SetCurrentFile(view().saveFileDialog.GetPath());
    Save();
    return true;
}

bool MainPresenter::SaveOrSaveAs()
{
    if (currentFile.IsEmpty())
    {
        if (!SaveAs())
        {
            return false;
        }
    }
    else
    {
        Save();
    }

    return true;
}

void MainPresenter::SetCurrentFile(const wxString &path)
{
    currentFile = path;

    if (currentFile.IsEmpty())
    {
        view().SetTitle("Ted");
        view().textArea->Clear();
    }
    else
    {
        view().SetTitle(currentFile + " - Ted");
    }
}
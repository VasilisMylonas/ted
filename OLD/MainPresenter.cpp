#include "MainPresenter.hpp"

#include "MainFrame.hpp"

MainPresenter::MainPresenter(MainFrame &view)
    : Presenter(view)
{
}

void MainPresenter::Init()
{
    View().LoadHistory(*wxConfig::Get());
}

void MainPresenter::save()
{
    auto selected = View().GetSelected();
    if (selected)
    {
        selected.value()->Save();
    }
}

bool MainPresenter::saveAs()
{
    auto path = View().ShowSaveDialog();

    if (!path)
    {
        return false;
    }

    save();
    return true;
}

void MainPresenter::OnNew([[maybe_unused]] wxCommandEvent &event)
{
    View().AddDocument();
}

void MainPresenter::OnClose([[maybe_unused]] wxCommandEvent &event)
{
    auto selected = View().GetSelected();
    if (!selected || selected.value()->IsModified())
    {
        // TODO save dialog
        View().RemoveSelected();
        return;
    }
}

void MainPresenter::OnOpen([[maybe_unused]] wxCommandEvent &event)
{
    auto path = View().ShowOpenDialog();

    if (!path)
    {
        return;
    }

    View().AddDocument(*path);
    View().AddHistoryEntry(*path);
}

void MainPresenter::OnSave([[maybe_unused]] wxCommandEvent &event)
{
    save();
}

void MainPresenter::OnSaveAs([[maybe_unused]] wxCommandEvent &event)
{
    saveAs();
}

void MainPresenter::quit()
{
    View().SaveHistory(*wxConfig::Get());
}

/*
bool MainPresenter::SaveUnsavedChanges()
{
    if (currentFile.empty())
    {
        return saveAs();
    }

    Save();
    return true;
}
*/

void MainPresenter::closeDocument(DocumentView *documentView)
{
    auto result = View().ShowUnsavedChangesDialog();

    // Cancel
    if (!result)
    {
        return;
    }

    if (!result.value() || SaveUnsavedChanges())
    {
        // Quit();
        // event.Skip();
        return;
    }
}

void MainPresenter::OnCloseWindow([[maybe_unused]] wxCloseEvent &event)
{
    std::optional<DocumentView *> selected;
    while ((selected = View().GetSelected()).has_value())
    {
        closeDocument(selected.value());
    }
}

void MainPresenter::OnQuit([[maybe_unused]] wxCommandEvent &event)
{
    View().Close();
}

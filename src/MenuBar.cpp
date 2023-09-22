#include "MenuBar.hpp"

MenuBar::MenuBar()
    : wxMenuBar()
{
    wxMenu *file = new wxMenu();
    file->Append(wxID_OPEN);
    file->AppendSeparator();
    file->Append(wxID_SAVE);
    file->Append(wxID_SAVEAS);
    file->AppendSeparator();
    file->Append(wxID_EXIT);

    Append(file, "&File");
}
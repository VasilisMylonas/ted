#include "MenuBar.hpp"

MenuBar::MenuBar()
    : wxMenuBar()
{
    wxMenu *file = new wxMenu();
    file->Append(wxID_OPEN);
    file->Append(wxID_EXIT);

    Append(file, "&File");
}
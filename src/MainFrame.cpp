#include "MainFrame.hpp"

#include "MenuBar.hpp"

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Ted")
{
    SetMenuBar(new MenuBar());
}

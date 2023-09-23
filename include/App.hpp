#pragma once

#include "Core.hpp"

class App : public wxApp
{
    virtual bool OnInit() override;
};

wxDECLARE_APP(App);
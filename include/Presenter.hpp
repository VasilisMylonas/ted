#pragma once

#include "Core.hpp"

template <typename TView>
class Presenter
{
public:
    Presenter(TView &view) : view{view}
    {
    }

    TView &View()
    {
        return view;
    }

private:
    TView &view;
};

#pragma once

template <typename View>
class Presenter
{
public:
    Presenter(View &view) : view_{view}
    {
    }

    View &view()
    {
        return view_;
    }

private:
    View &view_;
};

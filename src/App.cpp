#include "App.hpp"
#include "MainFrame.hpp"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
  auto frame = new MainFrame();
  frame->Show();
  return true;
}

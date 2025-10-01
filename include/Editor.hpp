#pragma once

#include <optional>
#include <string>
#include <wx/stc/stc.h>
#include <wx/wx.h>

class Editor : public wxPanel {
public:
  Editor(wxWindow *parent);
  Editor(wxWindow *parent, const std::string &path);

  void Load(const std::string &path);
  void Save();
  void SaveAs();
  void Close();
  bool IsModified();
  std::string GetTitle();

private:
  bool ShowUnsavedChangesDialog();
  std::optional<std::string> ShowSaveFileDialog();

  wxMessageDialog unsavedChangesDialog{
      this,
      wxT("You have unsaved changes. Do you want to save them?"),
      wxT("Unsaved Changes"),
      wxYES_NO | wxICON_WARNING,
  };

  wxFileDialog saveFileDialog{
      this,          wxT("Save File"),      wxEmptyString,
      wxEmptyString, wxT("Any File (*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
  };

  wxStyledTextCtrl *textCtrl;
  std::string path;
};

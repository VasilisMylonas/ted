#pragma once

#include <optional>
#include <string>
#include <wx/fdrepdlg.h>
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

  void Paste();
  void Copy();
  void Cut();
  void Undo();
  void Redo();
  void Find();
  void Replace();

private:
  void OnChar(wxKeyEvent &event);
  void OnFindDialogEvents(wxFindDialogEvent &event);
  void OnCaretPositionChanged(wxStyledTextEvent &event);
  bool ShowUnsavedChangesDialog();
  std::optional<std::string> ShowSaveFileDialog();
  void ShowFindDialog();
  void ShowReplaceDialog();

  // Syntax highlighting methods
  void SetupEditor();
  void SetSyntaxHighlighting(const std::string &filename);
  void ApplyStyle(int lexer);
  void StyleClearAll();

  // Get language name from lexer type
  wxString GetLanguageName() const;

  // Search flags and data
  int searchFlags = 0;
  wxString findText;
  wxString replaceText;

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

  wxFindReplaceDialog *findDialog = nullptr;
  wxFindReplaceDialog *replaceDialog = nullptr;
  wxFindReplaceData findReplaceData;

  wxStyledTextCtrl *textCtrl;
  std::string path;
  int currentLexer = wxSTC_LEX_NULL; // Current syntax highlighter
};

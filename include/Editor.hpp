#pragma once

#include <fstream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
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
  void DoFindReplace(int searchFlags, const std::string &findText,
                     bool next = false, bool replace = false,
                     const std::string &replaceText = "",
                     bool replaceAll = false);

  void OnCaretPositionChanged(wxStyledTextEvent &event);

  void OnFindDialogClose(wxFindDialogEvent &event);
  void OnFind(wxFindDialogEvent &event);
  void OnFindNext(wxFindDialogEvent &event);
  void OnFindReplace(wxFindDialogEvent &event);
  void OnFindReplaceAll(wxFindDialogEvent &event);

  bool ShowUnsavedChangesDialog();
  std::optional<std::string> ShowSaveFileDialog();

  // Syntax highlighting methods
  // void SetupEditor();
  // void SetSyntaxHighlighting(const std::string &filename);
  // void ApplyStyle(int lexer);
  // void StyleClearAll();

  // Theme methods
  // void LoadTheme(const std::string &themeName);
  // bool LoadThemeFromFile(const std::string &filename);
  // static std::string GetDefaultThemesDirectory();

  // struct StyleInfo {
  //   wxColour foreground;
  //   wxColour background;
  //   bool bold;
  //   bool italic;
  //   bool underline;
  // };

  // struct ThemeData {
  //   std::string name;
  //   wxColour defaultForeground;
  //   wxColour defaultBackground;
  //   wxColour selectionBackground;
  //   wxColour caretForeground;
  //   wxColour edgeColor;
  //   wxColour marginBackground;
  //   std::unordered_map<std::string, StyleInfo> styles;
  // };

  // ThemeData currentTheme;

  // static std::string GetLanguageNameFromLexer(int lexer);
  // static int GetLexerFromFileExtension(const std::string &fileExtension);
  // int currentLexer = wxSTC_LEX_NULL; // Current syntax highlighter
  // std::string currentThemeName = "default";

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
};

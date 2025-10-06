#include "Editor.hpp"
#include <wx/event.h>
#include <wx/notebook.h>

Editor::Editor(wxWindow *parent) : wxPanel(parent) {
  auto sizer = new wxBoxSizer(wxVERTICAL);
  textCtrl = new wxStyledTextCtrl(this, wxID_ANY);
  sizer->Add(textCtrl, 1, wxEXPAND);
  SetSizerAndFit(sizer);

  // Bind events
  textCtrl->Bind(wxEVT_CHAR, &Editor::OnChar, this);
  textCtrl->Bind(wxEVT_STC_UPDATEUI, &Editor::OnCaretPositionChanged, this);

  // Set up the editor with default settings and style
  SetupEditor();
}

Editor::Editor(wxWindow *parent, const std::string &path) : Editor(parent) {
  Load(path);
}

void Editor::Close() {
  if (!textCtrl->GetModify()) {
    return;
  }

  if (ShowUnsavedChangesDialog()) {
    Save();
  }
}

void Editor::Load(const std::string &path) {
  this->path = path;
  textCtrl->LoadFile(path);

  // Set syntax highlighting based on file extension
  SetSyntaxHighlighting(path);
}

void Editor::Save() {
  if (path.empty()) {
    auto newPath = ShowSaveFileDialog();
    if (!newPath) {
      return;
    }

    path = newPath.value();
  }

  textCtrl->SaveFile(path);
}

void Editor::SaveAs() {
  auto newPath = ShowSaveFileDialog();
  if (!newPath) {
    return;
  }

  path = newPath.value();
  textCtrl->SaveFile(path);
}

bool Editor::IsModified() { return textCtrl->GetModify(); }

std::string Editor::GetTitle() {
  if (path.empty()) {
    return "Untitled";
  }

  auto pos = path.find_last_of("/\\");
  if (pos == std::string::npos) {
    return path;
  }

  return path.substr(pos + 1);
}

bool Editor::ShowUnsavedChangesDialog() {
  if (unsavedChangesDialog.ShowModal() == wxID_YES) {
    return true;
  }
  return false;
}

std::optional<std::string> Editor::ShowSaveFileDialog() {
  if (saveFileDialog.ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }
  return saveFileDialog.GetPath().ToStdString();
}

void Editor::Paste() { textCtrl->Paste(); }

void Editor::Copy() { textCtrl->Copy(); }

void Editor::Cut() { textCtrl->Cut(); }

void Editor::Undo() { textCtrl->Undo(); }

void Editor::Redo() { textCtrl->Redo(); }

void Editor::Find() { ShowFindDialog(); }

void Editor::Replace() { ShowReplaceDialog(); }

void Editor::ShowFindDialog() {
  if (findDialog) {
    findDialog->SetFocus();
    return;
  }

  findDialog = new wxFindReplaceDialog(this, &findReplaceData, wxT("Find"),
                                       wxFR_NOUPDOWN | wxFR_NOWHOLEWORD |
                                           wxFR_NOMATCHCASE);

  findDialog->Bind(wxEVT_FIND, &Editor::OnFindDialogEvents, this);
  findDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindDialogEvents, this);
  findDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogEvents, this);

  findDialog->Show();
}

void Editor::ShowReplaceDialog() {
  if (replaceDialog) {
    replaceDialog->SetFocus();
    return;
  }

  replaceDialog = new wxFindReplaceDialog(
      this, &findReplaceData, wxT("Find and Replace"),
      wxFR_REPLACEDIALOG | wxFR_NOUPDOWN | wxFR_NOWHOLEWORD | wxFR_NOMATCHCASE);

  replaceDialog->Bind(wxEVT_FIND, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_NEXT, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE, &Editor::OnFindDialogEvents, this);
  replaceDialog->Bind(wxEVT_FIND_REPLACE_ALL, &Editor::OnFindDialogEvents,
                      this);
  replaceDialog->Bind(wxEVT_FIND_CLOSE, &Editor::OnFindDialogEvents, this);

  replaceDialog->Show();
}

void Editor::OnFindDialogEvents(wxFindDialogEvent &event) {
  wxEventType type = event.GetEventType();

  if (type == wxEVT_FIND_CLOSE) {
    wxFindReplaceDialog *dlg = event.GetDialog();
    if (dlg == findDialog) {
      findDialog->Destroy();
      findDialog = nullptr;
    } else if (dlg == replaceDialog) {
      replaceDialog->Destroy();
      replaceDialog = nullptr;
    }
    return;
  }

  // Get the search text and flags
  findText = event.GetFindString();
  int flags = event.GetFlags();
  searchFlags = 0;

  if (flags & wxFR_MATCHCASE) {
    searchFlags |= wxSTC_FIND_MATCHCASE;
  }
  if (flags & wxFR_WHOLEWORD) {
    searchFlags |= wxSTC_FIND_WHOLEWORD;
  }

  // Current position and selection
  int currentPos = textCtrl->GetCurrentPos();
  int selectionStart = textCtrl->GetSelectionStart();
  int selectionEnd = textCtrl->GetSelectionEnd();
  int searchStartPos;

  // Set the search start position
  if (type == wxEVT_FIND) {
    // For a new search, start from the current caret position
    searchStartPos = currentPos;
  } else if (type == wxEVT_FIND_NEXT) {
    // For Find Next, start from after the current selection
    searchStartPos = selectionEnd;
  } else {
    // For replace events, use the current selection position
    searchStartPos = selectionStart;
  }

  // Find and replace actions
  if (type == wxEVT_FIND || type == wxEVT_FIND_NEXT) {
    // Simple find operation
    textCtrl->SetSearchFlags(searchFlags);
    textCtrl->SetTargetStart(searchStartPos);
    textCtrl->SetTargetEnd(textCtrl->GetTextLength());

    int pos = textCtrl->SearchInTarget(findText);
    if (pos >= 0) {
      textCtrl->SetSelection(pos, pos + findText.length());
      textCtrl->EnsureCaretVisible();
    } else {
      // If not found from the current position, try from the beginning
      if (searchStartPos > 0) {
        textCtrl->SetTargetStart(0);
        textCtrl->SetTargetEnd(textCtrl->GetTextLength());
        pos = textCtrl->SearchInTarget(findText);
        if (pos >= 0) {
          textCtrl->SetSelection(pos, pos + findText.length());
          textCtrl->EnsureCaretVisible();
          wxMessageBox(wxT("Search wrapped to the beginning of the document"),
                       wxT("Find"), wxOK | wxICON_INFORMATION);
        } else {
          wxMessageBox(wxT("Text not found"), wxT("Find"),
                       wxOK | wxICON_INFORMATION);
        }
      } else {
        wxMessageBox(wxT("Text not found"), wxT("Find"),
                     wxOK | wxICON_INFORMATION);
      }
    }
  } else if (type == wxEVT_FIND_REPLACE) {
    // Replace the current selection if it matches the find string
    wxString currentSelection = textCtrl->GetSelectedText();

    if (!currentSelection.IsEmpty() &&
        ((flags & wxFR_MATCHCASE && currentSelection == findText) ||
         (!(flags & wxFR_MATCHCASE) &&
          currentSelection.Lower() == findText.Lower()))) {
      replaceText = event.GetReplaceString();
      textCtrl->ReplaceSelection(replaceText);
      textCtrl->SetSelection(selectionStart,
                             selectionStart + replaceText.length());

      // Find the next occurrence
      textCtrl->SetSearchFlags(searchFlags);
      textCtrl->SetTargetStart(selectionStart + replaceText.length());
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      int pos = textCtrl->SearchInTarget(findText);
      if (pos >= 0) {
        textCtrl->SetSelection(pos, pos + findText.length());
        textCtrl->EnsureCaretVisible();
      }
    } else {
      // If current selection doesn't match, perform a find operation
      textCtrl->SetSearchFlags(searchFlags);
      textCtrl->SetTargetStart(currentPos);
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      int pos = textCtrl->SearchInTarget(findText);
      if (pos >= 0) {
        textCtrl->SetSelection(pos, pos + findText.length());
        textCtrl->EnsureCaretVisible();
      }
    }
  } else if (type == wxEVT_FIND_REPLACE_ALL) {
    // Replace all occurrences of the find string
    int count = 0;
    replaceText = event.GetReplaceString();

    textCtrl->BeginUndoAction();

    textCtrl->SetSearchFlags(searchFlags);
    textCtrl->SetTargetStart(0);
    textCtrl->SetTargetEnd(textCtrl->GetTextLength());

    int pos = textCtrl->SearchInTarget(findText);
    while (pos >= 0) {
      textCtrl->SetTargetStart(pos);
      textCtrl->SetTargetEnd(pos + findText.length());
      textCtrl->ReplaceTarget(replaceText);
      count++;

      // Continue search from the end of replacement
      textCtrl->SetTargetStart(pos + replaceText.length());
      textCtrl->SetTargetEnd(textCtrl->GetTextLength());
      pos = textCtrl->SearchInTarget(findText);
    }

    textCtrl->EndUndoAction();

    wxString message = wxString::Format(wxT("Replaced %d occurrences"), count);
    wxMessageBox(message, wxT("Replace All"), wxOK | wxICON_INFORMATION);
  }
}

void Editor::OnChar(wxKeyEvent &event) {
  if (event.GetKeyCode() == WXK_SPACE) {
    textCtrl->BeginUndoAction();
    event.Skip();
    textCtrl->EndUndoAction();
  } else {
    event.Skip();
  }
}

void Editor::SetupEditor() {
  // Set up basic editor properties
  textCtrl->SetTabWidth(4);
  textCtrl->SetUseTabs(false);
  textCtrl->SetBackSpaceUnIndents(true);
  textCtrl->SetTabIndents(true);
  textCtrl->SetIndent(4);
  textCtrl->SetIndentationGuides(true);

  // Line numbers and margins
  textCtrl->SetMarginWidth(0,
                           textCtrl->TextWidth(wxSTC_STYLE_LINENUMBER, "_999"));
  textCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);

  // Code folding margin
  textCtrl->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
  textCtrl->SetMarginWidth(1, 16);
  textCtrl->SetMarginMask(1, wxSTC_MASK_FOLDERS);
  textCtrl->SetMarginSensitive(1, true);

  // Folding markers
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);

  // Other visual elements
  textCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
  textCtrl->SetEdgeColumn(80);
  textCtrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
  textCtrl->SetViewEOL(false);

  // Enable line wrapping
  textCtrl->SetWrapMode(wxSTC_WRAP_WORD);

  // Enable brace matching
  textCtrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);

  // Apply default style (plain text)
  ApplyStyle(wxSTC_LEX_NULL);
}

void Editor::SetSyntaxHighlighting(const std::string &filename) {
  if (filename.empty()) {
    ApplyStyle(wxSTC_LEX_NULL);
    return;
  }

  // Find the extension
  size_t dotPos = filename.find_last_of('.');
  if (dotPos == std::string::npos) {
    ApplyStyle(wxSTC_LEX_NULL); // No extension, use plain text
    return;
  }

  std::string ext = filename.substr(dotPos);

  // Convert to lowercase for case-insensitive comparison
  for (auto &c : ext) {
    c = tolower(c);
  }

  // Detect language based on extension
  if (ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".h" ||
      ext == ".hpp" || ext == ".hxx") {
    ApplyStyle(wxSTC_LEX_CPP);
  } else if (ext == ".py") {
    ApplyStyle(wxSTC_LEX_PYTHON);
  } else if (ext == ".js") {
    ApplyStyle(wxSTC_LEX_CPP); // Use C++ lexer for JavaScript
  } else if (ext == ".html" || ext == ".htm") {
    ApplyStyle(wxSTC_LEX_HTML);
  } else if (ext == ".xml") {
    ApplyStyle(wxSTC_LEX_XML);
  } else if (ext == ".css") {
    ApplyStyle(wxSTC_LEX_CSS);
  } else if (ext == ".sql") {
    ApplyStyle(wxSTC_LEX_SQL);
  } else if (ext == ".java") {
    ApplyStyle(wxSTC_LEX_CPP); // Java uses similar highlighting to C++
  } else if (ext == ".rb") {
    ApplyStyle(wxSTC_LEX_RUBY);
  } else if (ext == ".php") {
    ApplyStyle(wxSTC_LEX_PHPSCRIPT);
  } else if (ext == ".md" || ext == ".markdown") {
    ApplyStyle(wxSTC_LEX_MARKDOWN);
  } else if (ext == ".json") {
    ApplyStyle(wxSTC_LEX_JSON);
  } else if (ext == ".txt" || ext == ".text") {
    ApplyStyle(wxSTC_LEX_NULL);
  } else {
    // Default to plain text for unknown extensions
    ApplyStyle(wxSTC_LEX_NULL);
  }
}

void Editor::StyleClearAll() {
  for (int i = 0; i < wxSTC_STYLE_MAX; i++) {
    textCtrl->StyleSetForeground(i, *wxBLACK);
    textCtrl->StyleSetBackground(i, *wxWHITE);
    textCtrl->StyleSetFont(i, wxFont(10, wxFONTFAMILY_MODERN,
                                     wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->StyleSetBold(i, false);
    textCtrl->StyleSetItalic(i, false);
    textCtrl->StyleSetUnderline(i, false);
  }
}

void Editor::ApplyStyle(int lexer) {
  // Save the current lexer
  currentLexer = lexer;

  // Set lexer
  textCtrl->SetLexer(lexer);

  // Reset all styles
  StyleClearAll();

  // Set common base font for all styles
  wxFont baseFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_NORMAL);
  textCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, baseFont);
  textCtrl->StyleClearAll();

  // Set different styles based on lexer
  switch (lexer) {
  case wxSTC_LEX_CPP: {
    // Set keywords for C/C++
    textCtrl->SetKeyWords(0,
                          "auto break case char const continue default do "
                          "double else enum extern float for goto if int long "
                          "register return short signed sizeof static struct "
                          "switch typedef union unsigned void volatile while "
                          "class namespace template try catch throw new delete "
                          "using inline virtual public protected private "
                          "friend operator true false nullptr this");

    // Comments
    textCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0)); // Green
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 0));
    textCtrl->StyleSetItalic(wxSTC_C_COMMENT, true);
    textCtrl->StyleSetItalic(wxSTC_C_COMMENTLINE, true);
    textCtrl->StyleSetItalic(wxSTC_C_COMMENTDOC, true);

    // Numbers
    textCtrl->StyleSetForeground(wxSTC_C_NUMBER,
                                 wxColour(128, 0, 128)); // Purple

    // Keywords
    textCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255)); // Blue
    textCtrl->StyleSetBold(wxSTC_C_WORD, true);

    // Double-quoted strings
    textCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(163, 21, 21)); // Red

    // Single-quoted strings
    textCtrl->StyleSetForeground(wxSTC_C_CHARACTER,
                                 wxColour(163, 21, 21)); // Red

    // Preprocessor
    textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR,
                                 wxColour(128, 64, 0)); // Brown

    // Operators
    textCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 0)); // Black
    textCtrl->StyleSetBold(wxSTC_C_OPERATOR, true);
    break;
  }

  case wxSTC_LEX_PYTHON: {
    // Set keywords for Python
    textCtrl->SetKeyWords(
        0, "and as assert break class continue def del elif else except "
           "exec finally for from global if import in is lambda not "
           "or pass print raise return try while with yield");

    // Comments
    textCtrl->StyleSetForeground(wxSTC_P_COMMENTLINE, wxColour(0, 128, 0));
    textCtrl->StyleSetItalic(wxSTC_P_COMMENTLINE, true);

    // Numbers
    textCtrl->StyleSetForeground(wxSTC_P_NUMBER, wxColour(128, 0, 128));

    // Strings
    textCtrl->StyleSetForeground(wxSTC_P_STRING, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_CHARACTER, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_TRIPLE, wxColour(163, 21, 21));
    textCtrl->StyleSetForeground(wxSTC_P_TRIPLEDOUBLE, wxColour(163, 21, 21));

    // Keywords
    textCtrl->StyleSetForeground(wxSTC_P_WORD, wxColour(0, 0, 255));
    textCtrl->StyleSetBold(wxSTC_P_WORD, true);

    // Class names
    textCtrl->StyleSetForeground(wxSTC_P_CLASSNAME, wxColour(0, 64, 128));
    textCtrl->StyleSetBold(wxSTC_P_CLASSNAME, true);

    // Function names
    textCtrl->StyleSetForeground(wxSTC_P_DEFNAME, wxColour(0, 128, 128));
    textCtrl->StyleSetBold(wxSTC_P_DEFNAME, true);

    // Operators
    textCtrl->StyleSetForeground(wxSTC_P_OPERATOR, wxColour(0, 0, 0));
    textCtrl->StyleSetBold(wxSTC_P_OPERATOR, true);
    break;
  }

  case wxSTC_LEX_HTML:
  case wxSTC_LEX_XML: {
    // Keywords for HTML/XML
    textCtrl->SetKeyWords(0, "html body head title div span p h1 h2 h3 h4 h5 "
                             "h6 ul ol li a img table tr td th");

    // Tags
    textCtrl->StyleSetForeground(wxSTC_H_TAG, wxColour(0, 0, 128)); // Dark blue
    textCtrl->StyleSetBold(wxSTC_H_TAG, true);

    // Attributes
    textCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTE,
                                 wxColour(0, 0, 255)); // Blue

    // Strings
    textCtrl->StyleSetForeground(wxSTC_H_DOUBLESTRING,
                                 wxColour(163, 21, 21)); // Red
    textCtrl->StyleSetForeground(wxSTC_H_SINGLESTRING,
                                 wxColour(163, 21, 21)); // Red

    // Comments
    textCtrl->StyleSetForeground(wxSTC_H_COMMENT, wxColour(0, 128, 0)); // Green
    textCtrl->StyleSetItalic(wxSTC_H_COMMENT, true);
    break;
  }

    // JavaScript is handled by the CPP lexer in the wxSTC_LEX_CPP case above

    // More languages can be added as needed

  default:
    // Plain text - no special styling
    break;
  }
}

void Editor::OnCaretPositionChanged(wxStyledTextEvent &event) {
  // Get current position information
  int pos = textCtrl->GetCurrentPos();
  int line = textCtrl->LineFromPosition(pos);
  int col = textCtrl->GetColumn(pos);

  // Emit custom event to notify the parent (MainFrame)
  wxCommandEvent statusEvent(wxEVT_COMMAND_TEXT_UPDATED, GetId());
  statusEvent.SetInt(
      line +
      1); // Line numbers are 0-based internally, but displayed as 1-based
  statusEvent.SetExtraLong(col + 1); // Column numbers are also 0-based
  statusEvent.SetString(GetLanguageName());

  // Propagate the event up
  wxPostEvent(GetParent(), statusEvent);

  event.Skip();
}

wxString Editor::GetLanguageName() const {
  switch (currentLexer) {
  case wxSTC_LEX_CPP:
    if (path.find(".js") != std::string::npos) {
      return "JavaScript";
    }
    return "C++";
  case wxSTC_LEX_PYTHON:
    return "Python";
  case wxSTC_LEX_HTML:
    return "HTML";
  case wxSTC_LEX_XML:
    return "XML";
  case wxSTC_LEX_CSS:
    return "CSS";
  case wxSTC_LEX_SQL:
    return "SQL";
  case wxSTC_LEX_RUBY:
    return "Ruby";
  case wxSTC_LEX_MARKDOWN:
    return "Markdown";
  case wxSTC_LEX_JSON:
    return "JSON";
  case wxSTC_LEX_NULL:
    if (path.empty()) {
      return "Text";
    }

    // Check for text file extensions
    if (path.find(".txt") != std::string::npos ||
        path.find(".text") != std::string::npos) {
      return "Text";
    }

    // For other files without recognized extensions
    return "Plain Text";
  default:
    return "Plain Text";
  }
}

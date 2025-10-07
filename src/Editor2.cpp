#include <fstream>
#include <sstream>
#include <unordered_map>

// --- Editor Setup and Syntax Highlighting ---
void Editor::SetupEditor() {
  textCtrl->SetTabWidth(4);
  textCtrl->SetUseTabs(false);
  textCtrl->SetBackSpaceUnIndents(true);
  textCtrl->SetTabIndents(true);
  textCtrl->SetIndent(4);
  textCtrl->SetIndentationGuides(true);
  textCtrl->SetMarginWidth(0,
                           textCtrl->TextWidth(wxSTC_STYLE_LINENUMBER, "_999"));
  textCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
  textCtrl->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
  textCtrl->SetMarginWidth(1, 16);
  textCtrl->SetMarginMask(1, wxSTC_MASK_FOLDERS);
  textCtrl->SetMarginSensitive(1, true);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
  textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
  textCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
  textCtrl->SetEdgeColumn(80);
  textCtrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
  textCtrl->SetViewEOL(false);
  textCtrl->SetWrapMode(wxSTC_WRAP_WORD);
  textCtrl->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
  ApplyStyle(wxSTC_LEX_NULL);
}

void Editor::SetSyntaxHighlighting(const std::string &filename) {
  if (filename.empty()) {
    ApplyStyle(wxSTC_LEX_NULL);
    return;
  }
  size_t dotPos = filename.find_last_of('.');
  if (dotPos == std::string::npos) {
    ApplyStyle(wxSTC_LEX_NULL);
    return;
  }

  auto ext = filename.substr(dotPos);
  for (auto &c : ext) {
    c = tolower(c);
  }

  ApplyStyle(GetLexerFromFileExtension(ext));
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
  currentLexer = lexer;
  textCtrl->SetLexer(lexer);
  StyleClearAll();

  // Set up basic editor colors from theme
  textCtrl->StyleSetBackground(wxSTC_STYLE_DEFAULT,
                               currentTheme.defaultBackground);
  textCtrl->StyleSetForeground(wxSTC_STYLE_DEFAULT,
                               currentTheme.defaultForeground);

  wxFont baseFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_NORMAL);
  textCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, baseFont);

  // Apply selection and caret colors
  textCtrl->SetSelBackground(true, currentTheme.selectionBackground);
  textCtrl->SetCaretForeground(currentTheme.caretForeground);

  // Set edge color
  textCtrl->SetEdgeColour(currentTheme.edgeColor);

  // Set margin colors
  textCtrl->StyleSetBackground(wxSTC_STYLE_LINENUMBER,
                               currentTheme.marginBackground);

  // Apply the default style to all styles
  textCtrl->StyleClearAll();

  switch (lexer) {
  case wxSTC_LEX_CPP: {
    textCtrl->SetKeyWords(
        0,
        "auto break case char const continue default do double else enum "
        "extern float for goto if int long register return short signed sizeof "
        "static struct switch typedef union unsigned void volatile while class "
        "namespace template try catch throw new delete using inline virtual "
        "public protected private friend operator true false nullptr this");

    // Apply themed styles if available, otherwise fall back to defaults
    // Comments
    if (currentTheme.styles.find("comment") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["comment"];
      textCtrl->StyleSetForeground(wxSTC_C_COMMENT, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_COMMENT, style.background);
      textCtrl->StyleSetBold(wxSTC_C_COMMENT, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_COMMENT, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_COMMENT, style.underline);

      textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_COMMENTLINE, style.background);
      textCtrl->StyleSetBold(wxSTC_C_COMMENTLINE, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_COMMENTLINE, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_COMMENTLINE, style.underline);

      textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_COMMENTDOC, style.background);
      textCtrl->StyleSetBold(wxSTC_C_COMMENTDOC, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_COMMENTDOC, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_COMMENTDOC, style.underline);
    } else {
      // Fallback to hardcoded style
      textCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
      textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
      textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 0));
      textCtrl->StyleSetItalic(wxSTC_C_COMMENT, true);
      textCtrl->StyleSetItalic(wxSTC_C_COMMENTLINE, true);
      textCtrl->StyleSetItalic(wxSTC_C_COMMENTDOC, true);
    }

    // Numbers
    if (currentTheme.styles.find("number") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["number"];
      textCtrl->StyleSetForeground(wxSTC_C_NUMBER, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_NUMBER, style.background);
      textCtrl->StyleSetBold(wxSTC_C_NUMBER, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_NUMBER, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_NUMBER, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_C_NUMBER, wxColour(128, 0, 128));
    }

    // Keywords
    if (currentTheme.styles.find("keyword") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["keyword"];
      textCtrl->StyleSetForeground(wxSTC_C_WORD, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_WORD, style.background);
      textCtrl->StyleSetBold(wxSTC_C_WORD, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_WORD, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_WORD, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
      textCtrl->StyleSetBold(wxSTC_C_WORD, true);
    }

    // Strings
    if (currentTheme.styles.find("string") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["string"];
      textCtrl->StyleSetForeground(wxSTC_C_STRING, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_STRING, style.background);
      textCtrl->StyleSetBold(wxSTC_C_STRING, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_STRING, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_STRING, style.underline);

      textCtrl->StyleSetForeground(wxSTC_C_CHARACTER, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_CHARACTER, style.background);
      textCtrl->StyleSetBold(wxSTC_C_CHARACTER, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_CHARACTER, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_CHARACTER, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(163, 21, 21));
      textCtrl->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(163, 21, 21));
    }

    // Preprocessor
    if (currentTheme.styles.find("preprocessor") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["preprocessor"];
      textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_PREPROCESSOR, style.background);
      textCtrl->StyleSetBold(wxSTC_C_PREPROCESSOR, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_PREPROCESSOR, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_PREPROCESSOR, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 64, 0));
    }

    // Operators
    if (currentTheme.styles.find("operator") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["operator"];
      textCtrl->StyleSetForeground(wxSTC_C_OPERATOR, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_C_OPERATOR, style.background);
      textCtrl->StyleSetBold(wxSTC_C_OPERATOR, style.bold);
      textCtrl->StyleSetItalic(wxSTC_C_OPERATOR, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_C_OPERATOR, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 0));
      textCtrl->StyleSetBold(wxSTC_C_OPERATOR, true);
    }
    break;
  }
  case wxSTC_LEX_PYTHON: {
    textCtrl->SetKeyWords(
        0, "and as assert break class continue def del elif else except exec "
           "finally for from global if import in is lambda not or pass print "
           "raise return try while with yield");

    // Comments
    if (currentTheme.styles.find("comment") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["comment"];
      textCtrl->StyleSetForeground(wxSTC_P_COMMENTLINE, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_COMMENTLINE, style.background);
      textCtrl->StyleSetBold(wxSTC_P_COMMENTLINE, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_COMMENTLINE, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_COMMENTLINE, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_COMMENTLINE, wxColour(0, 128, 0));
      textCtrl->StyleSetItalic(wxSTC_P_COMMENTLINE, true);
    }

    // Numbers
    if (currentTheme.styles.find("number") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["number"];
      textCtrl->StyleSetForeground(wxSTC_P_NUMBER, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_NUMBER, style.background);
      textCtrl->StyleSetBold(wxSTC_P_NUMBER, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_NUMBER, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_NUMBER, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_NUMBER, wxColour(128, 0, 128));
    }

    // Strings
    if (currentTheme.styles.find("string") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["string"];
      textCtrl->StyleSetForeground(wxSTC_P_STRING, style.foreground);
      textCtrl->StyleSetForeground(wxSTC_P_CHARACTER, style.foreground);
      textCtrl->StyleSetForeground(wxSTC_P_TRIPLE, style.foreground);
      textCtrl->StyleSetForeground(wxSTC_P_TRIPLEDOUBLE, style.foreground);

      textCtrl->StyleSetBackground(wxSTC_P_STRING, style.background);
      textCtrl->StyleSetBackground(wxSTC_P_CHARACTER, style.background);
      textCtrl->StyleSetBackground(wxSTC_P_TRIPLE, style.background);
      textCtrl->StyleSetBackground(wxSTC_P_TRIPLEDOUBLE, style.background);

      textCtrl->StyleSetBold(wxSTC_P_STRING, style.bold);
      textCtrl->StyleSetBold(wxSTC_P_CHARACTER, style.bold);
      textCtrl->StyleSetBold(wxSTC_P_TRIPLE, style.bold);
      textCtrl->StyleSetBold(wxSTC_P_TRIPLEDOUBLE, style.bold);

      textCtrl->StyleSetItalic(wxSTC_P_STRING, style.italic);
      textCtrl->StyleSetItalic(wxSTC_P_CHARACTER, style.italic);
      textCtrl->StyleSetItalic(wxSTC_P_TRIPLE, style.italic);
      textCtrl->StyleSetItalic(wxSTC_P_TRIPLEDOUBLE, style.italic);

      textCtrl->StyleSetUnderline(wxSTC_P_STRING, style.underline);
      textCtrl->StyleSetUnderline(wxSTC_P_CHARACTER, style.underline);
      textCtrl->StyleSetUnderline(wxSTC_P_TRIPLE, style.underline);
      textCtrl->StyleSetUnderline(wxSTC_P_TRIPLEDOUBLE, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_STRING, wxColour(163, 21, 21));
      textCtrl->StyleSetForeground(wxSTC_P_CHARACTER, wxColour(163, 21, 21));
      textCtrl->StyleSetForeground(wxSTC_P_TRIPLE, wxColour(163, 21, 21));
      textCtrl->StyleSetForeground(wxSTC_P_TRIPLEDOUBLE, wxColour(163, 21, 21));
    }

    // Keywords
    if (currentTheme.styles.find("keyword") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["keyword"];
      textCtrl->StyleSetForeground(wxSTC_P_WORD, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_WORD, style.background);
      textCtrl->StyleSetBold(wxSTC_P_WORD, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_WORD, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_WORD, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_WORD, wxColour(0, 0, 255));
      textCtrl->StyleSetBold(wxSTC_P_WORD, true);
    }

    // Class name
    if (currentTheme.styles.find("class") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["class"];
      textCtrl->StyleSetForeground(wxSTC_P_CLASSNAME, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_CLASSNAME, style.background);
      textCtrl->StyleSetBold(wxSTC_P_CLASSNAME, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_CLASSNAME, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_CLASSNAME, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_CLASSNAME, wxColour(0, 64, 128));
      textCtrl->StyleSetBold(wxSTC_P_CLASSNAME, true);
    }

    // Function name
    if (currentTheme.styles.find("function") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["function"];
      textCtrl->StyleSetForeground(wxSTC_P_DEFNAME, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_DEFNAME, style.background);
      textCtrl->StyleSetBold(wxSTC_P_DEFNAME, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_DEFNAME, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_DEFNAME, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_DEFNAME, wxColour(0, 128, 128));
      textCtrl->StyleSetBold(wxSTC_P_DEFNAME, true);
    }

    // Operators
    if (currentTheme.styles.find("operator") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["operator"];
      textCtrl->StyleSetForeground(wxSTC_P_OPERATOR, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_P_OPERATOR, style.background);
      textCtrl->StyleSetBold(wxSTC_P_OPERATOR, style.bold);
      textCtrl->StyleSetItalic(wxSTC_P_OPERATOR, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_P_OPERATOR, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_P_OPERATOR, wxColour(0, 0, 0));
      textCtrl->StyleSetBold(wxSTC_P_OPERATOR, true);
    }
    break;
  }
  case wxSTC_LEX_HTML:
  case wxSTC_LEX_XML: {
    textCtrl->SetKeyWords(0, "html body head title div span p h1 h2 h3 h4 h5 "
                             "h6 ul ol li a img table tr td th");

    // HTML/XML Tags
    if (currentTheme.styles.find("tag") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["tag"];
      textCtrl->StyleSetForeground(wxSTC_H_TAG, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_H_TAG, style.background);
      textCtrl->StyleSetBold(wxSTC_H_TAG, style.bold);
      textCtrl->StyleSetItalic(wxSTC_H_TAG, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_H_TAG, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_H_TAG, wxColour(0, 0, 128));
      textCtrl->StyleSetBold(wxSTC_H_TAG, true);
    }

    // HTML/XML Attributes
    if (currentTheme.styles.find("attribute") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["attribute"];
      textCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTE, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_H_ATTRIBUTE, style.background);
      textCtrl->StyleSetBold(wxSTC_H_ATTRIBUTE, style.bold);
      textCtrl->StyleSetItalic(wxSTC_H_ATTRIBUTE, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_H_ATTRIBUTE, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTE, wxColour(0, 0, 255));
    }

    // HTML/XML Strings
    if (currentTheme.styles.find("string") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["string"];
      textCtrl->StyleSetForeground(wxSTC_H_DOUBLESTRING, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_H_DOUBLESTRING, style.background);
      textCtrl->StyleSetBold(wxSTC_H_DOUBLESTRING, style.bold);
      textCtrl->StyleSetItalic(wxSTC_H_DOUBLESTRING, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_H_DOUBLESTRING, style.underline);

      textCtrl->StyleSetForeground(wxSTC_H_SINGLESTRING, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_H_SINGLESTRING, style.background);
      textCtrl->StyleSetBold(wxSTC_H_SINGLESTRING, style.bold);
      textCtrl->StyleSetItalic(wxSTC_H_SINGLESTRING, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_H_SINGLESTRING, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_H_DOUBLESTRING, wxColour(163, 21, 21));
      textCtrl->StyleSetForeground(wxSTC_H_SINGLESTRING, wxColour(163, 21, 21));
    }

    // HTML/XML Comments
    if (currentTheme.styles.find("comment") != currentTheme.styles.end()) {
      const auto &style = currentTheme.styles["comment"];
      textCtrl->StyleSetForeground(wxSTC_H_COMMENT, style.foreground);
      textCtrl->StyleSetBackground(wxSTC_H_COMMENT, style.background);
      textCtrl->StyleSetBold(wxSTC_H_COMMENT, style.bold);
      textCtrl->StyleSetItalic(wxSTC_H_COMMENT, style.italic);
      textCtrl->StyleSetUnderline(wxSTC_H_COMMENT, style.underline);
    } else {
      textCtrl->StyleSetForeground(wxSTC_H_COMMENT, wxColour(0, 128, 0));
      textCtrl->StyleSetItalic(wxSTC_H_COMMENT, true);
    }
    break;
  }
  default:
    break;
  }
}

// --- Caret and Status ---

static std::unordered_map<int, std::string> languageMap = {
    {wxSTC_LEX_CPP, "C++"},
    {wxSTC_LEX_PYTHON, "Python"},
    {wxSTC_LEX_NULL, "Plain Text"},
};

static std::unordered_map<std::string, int> extensionMap = {
    {".cpp", wxSTC_LEX_CPP},  {".cc", wxSTC_LEX_CPP},
    {".cxx", wxSTC_LEX_CPP},  {".h", wxSTC_LEX_CPP},
    {".c", wxSTC_LEX_CPP},    {".hpp", wxSTC_LEX_CPP},
    {".hxx", wxSTC_LEX_CPP},  {".py", wxSTC_LEX_PYTHON},
    {".txt", wxSTC_LEX_NULL}, {".text", wxSTC_LEX_NULL},
};

int Editor::GetLexerFromFileExtension(const std::string &fileExtension) {
  return extensionMap.contains(fileExtension) ? extensionMap[fileExtension]
                                              : wxSTC_LEX_NULL;
}

std::string Editor::GetLanguageNameFromLexer(int lexer) {
  return languageMap.contains(lexer) ? languageMap.at(lexer) : "Unknown";
}

std::string Editor::GetDefaultThemesDirectory() {
  // In a real application, this could be platform specific
  // For now, we'll use the current directory
  return "./themes/";
}

bool Editor::LoadThemeFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    wxLogError("Failed to open theme file: %s", filename);
    return false;
  }

  ThemeData theme;
  std::string line;
  std::string currentSection;

  auto parseColor = [](const std::string &colorStr) -> wxColour {
    if (colorStr.size() == 7 && colorStr[0] == '#') {
      // Format #RRGGBB
      unsigned long rgb;
      if (std::sscanf(colorStr.c_str(), "#%lx", &rgb) == 1) {
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;
        return wxColour(r, g, b);
      }
    }
    return *wxBLACK; // Default color if parsing fails
  };

  while (std::getline(file, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Section headers
    if (line[0] == '[' && line.back() == ']') {
      currentSection = line.substr(1, line.size() - 2);
      continue;
    }

    // Key-value pairs
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);

    if (currentSection == "theme") {
      if (key == "name") {
        theme.name = value;
      } else if (key == "foreground") {
        theme.defaultForeground = parseColor(value);
      } else if (key == "background") {
        theme.defaultBackground = parseColor(value);
      } else if (key == "selection") {
        theme.selectionBackground = parseColor(value);
      } else if (key == "caret") {
        theme.caretForeground = parseColor(value);
      } else if (key == "edge") {
        theme.edgeColor = parseColor(value);
      } else if (key == "margin") {
        theme.marginBackground = parseColor(value);
      }
    } else if (currentSection == "styles") {
      // Style format: style.element=foreground,background,bold,italic,underline
      std::string styleType = key;
      std::vector<std::string> parts;
      std::stringstream ss(value);
      std::string part;

      while (std::getline(ss, part, ',')) {
        // Trim whitespace
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);
        parts.push_back(part);
      }

      if (parts.size() >= 5) {
        StyleInfo style;
        style.foreground = parseColor(parts[0]);
        style.background = parseColor(parts[1]);
        style.bold = (parts[2] == "true");
        style.italic = (parts[3] == "true");
        style.underline = (parts[4] == "true");
        theme.styles[styleType] = style;
      }
    }
  }

  // If we successfully loaded the theme
  if (!theme.name.empty()) {
    currentTheme = theme;
    currentThemeName = theme.name;
    return true;
  }

  return false;
}

void Editor::LoadTheme(const std::string &themeName) {
  // Try to load the theme file
  std::string themePath = GetDefaultThemesDirectory() + themeName + ".theme";

  if (!LoadThemeFromFile(themePath)) {
    wxLogError("Failed to load theme '%s', falling back to default", themeName);
    // Fall back to default theme (could be hardcoded)
    ThemeData defaultTheme;
    defaultTheme.name = "default";
    defaultTheme.defaultForeground = *wxBLACK;
    defaultTheme.defaultBackground = *wxWHITE;
    defaultTheme.selectionBackground = wxColour(173, 214, 255);
    defaultTheme.caretForeground = *wxBLACK;
    defaultTheme.edgeColor = wxColour(192, 192, 192);
    defaultTheme.marginBackground = wxColour(240, 240, 240);

    // Add some default styles
    StyleInfo defaultStyle;
    defaultStyle.foreground = *wxBLACK;
    defaultStyle.background = *wxWHITE;
    defaultStyle.bold = false;
    defaultStyle.italic = false;
    defaultStyle.underline = false;

    StyleInfo commentStyle;
    commentStyle.foreground = wxColour(0, 128, 0);
    commentStyle.background = *wxWHITE;
    commentStyle.bold = false;
    commentStyle.italic = true;
    commentStyle.underline = false;

    defaultTheme.styles["default"] = defaultStyle;
    defaultTheme.styles["comment"] = commentStyle;

    currentTheme = defaultTheme;
    currentThemeName = "default";
  }

  // Apply the theme to the current editor
  ApplyStyle(currentLexer);
}

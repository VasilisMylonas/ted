#include "TextDocument.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

TextDocument::TextDocument() : path(""), content(""), modified(false) {}

TextDocument::TextDocument(const std::string &path)
    : path(path), modified(false) {

  std::ifstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  content = std::string((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
}

void TextDocument::SaveAs(const std::string &path) {
  this->path = path;
  Save();
}

void TextDocument::Save() {
  if (path.empty()) {
    throw std::runtime_error("Cannot save document without a path");
  }

  std::ofstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open file for writing: " + path);
  }
  file << content;
  modified = false;
}

const std::string &TextDocument::GetPath() { return path; }

const std::string &TextDocument::GetContent() { return content; }

#pragma once

#include <string>

class TextDocument {
public:
  TextDocument();
  TextDocument(const std::string &path);

  void SaveAs(const std::string &path);
  void Save();
  const std::string &GetPath();
  const std::string &GetContent();

private:
  std::string path;
  std::string content;
  bool modified;
};

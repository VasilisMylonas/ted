#pragma once

#include "Core.hpp"

#include <filesystem>
#include <string>

class Document : public wxPanel
{
private:
    wxTextCtrl *content;
    std::string path;
    std::string title;

public:
    const std::string &Title()
    {
        return title;
    }

    const std::string &Path()
    {
        return path;
    }

    void Save()
    {
        content->SaveFile(path);
    }

    void SetPath(const std::string &path)
    {
        this->path = path;
        title = getFilename(path);
    }

    bool IsModified()
    {
        return content->IsModified();
    }

    Document(wxWindow *parent) : wxPanel(parent, wxID_ANY), title{"New Document"}
    {
        content = new wxTextCtrl(this,
                                 wxID_ANY,
                                 wxEmptyString,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxTE_WORDWRAP | wxTE_MULTILINE);

        auto sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(content, 1, wxEXPAND | wxALL, 4); // TODO: padding
        SetSizerAndFit(sizer);

        // TODO: font
        auto font = content->GetFont();
        font.SetPointSize(16);
        content->SetFont(font);
    }

    Document(wxWindow *parent, const std::string &path) : Document{parent}
    {
        SetPath(path);
        Reload();
    }

    void Discard()
    {
        content->DiscardEdits();
    }

    void Reload()
    {
        content->LoadFile(path);
    }

private:
    static std::string getFilename(const std::string &path)
    {
        return std::filesystem::path{path}.filename().c_str();
    }
};

#include "default_view.hpp"
#include <wx/commandlinkbutton.h>

#include <utility>

default_view::default_view(shared::archive::file_info info)
  : info(std::move(info))
{}

void default_view::setup_view(wxWindow* parent, sf::RenderWindow* window, ImGuiContext* guiContext)
{
  static auto first_time = true;

  auto* horizontal = new wxBoxSizer(wxHORIZONTAL);
  auto* sizer = new wxBoxSizer(wxVERTICAL);

  if (first_time)
  {
    auto file_path = std::filesystem::current_path() / "logo.png";

    auto* text = new wxStaticText(parent, wxID_ANY, "Welcome to 3Space Studio.");
    text->SetWindowStyle(wxALIGN_CENTRE_HORIZONTAL);
    auto font = text->GetFont();
    font.SetPointSize(18);
    text->SetFont(font);

    if (std::filesystem::exists(file_path))
    {
      auto image = wxImage(file_path.string(), wxBITMAP_TYPE_PNG);

      auto* logo = new wxStaticBitmap(parent, wxID_ANY, wxBitmap(image));

      sizer->AddStretchSpacer(4);
      sizer->Add(logo, 1, wxEXPAND, 0);
      sizer->Add(text, 1, wxEXPAND, 0);
      sizer->AddStretchSpacer(4);
    }
    else
    {
      sizer->AddStretchSpacer(4);
      sizer->Add(text, 2, wxEXPAND, 0);
      sizer->AddStretchSpacer(4);
    }

    horizontal->AddStretchSpacer(2);
    horizontal->Add(sizer, 2, wxEXPAND, 0);
    horizontal->AddStretchSpacer(2);

    parent->SetSizer(horizontal);

    first_time = false;
  }
  else if (auto extension = info.filename.extension().string(); !extension.empty())
  {
    extension = extension.substr(1);
    auto* text = new wxStaticText(parent, wxID_ANY, "This particular file is not yet supported by 3Space Studio.\nThough, you can still read about it on our wiki.\nClick the link below to find out more.");
    text->SetWindowStyle(wxALIGN_CENTRE_HORIZONTAL);

    std::string url = "https://github.com/matthew-rindel/3space-studio/wiki/" + extension;

    auto* command = new wxCommandLinkButton(parent, wxID_ANY, url);

    command->Bind(wxEVT_BUTTON, [=](auto&) {
      wxLaunchDefaultBrowser(url);
    });

    sizer->AddStretchSpacer(4);
    sizer->Add(text, 1, wxEXPAND, 0);
    sizer->Add(command, 1, wxEXPAND, 0);
    sizer->AddStretchSpacer(4);

    horizontal->AddStretchSpacer(2);
    horizontal->Add(sizer, 2, wxEXPAND, 0);
    horizontal->AddStretchSpacer(2);

    parent->SetSizer(horizontal);
  }
}
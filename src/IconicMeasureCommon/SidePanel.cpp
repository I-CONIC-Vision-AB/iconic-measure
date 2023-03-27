#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	wxPanel* area_panel = new wxPanel(this, wxID_ANY);
	wxSizer* area_sizer = new wxBoxSizer(wxHORIZONTAL);

	area_panel->SetBackgroundColour(wxColour(200, 200, 255));
	sizer = new wxBoxSizer(wxVERTICAL);

	area_text = new wxStaticText(area_panel, wxID_ANY, wxString("Area"), wxDefaultPosition, wxDefaultSize);
	area_value = new wxStaticText(area_panel, wxID_ANY, wxString("placeholder"), wxDefaultPosition, wxDefaultSize);
	
	area_sizer->Add(area_text, 0, wxALIGN_CENTER_VERTICAL | wxALL , 10);
	area_sizer->Add(area_value, 0, wxALIGN_CENTER_VERTICAL);

	sizer->Add(area_panel, 0, wxEXPAND | wxALL | wxALIGN_CENTER_HORIZONTAL, 20);
	
	area_panel->SetSizerAndFit(area_sizer);
	SetSizerAndFit(sizer);
}

void iconic::SidePanel::SetAreaText(wxString text) {
	area_value->SetLabel(text);
}

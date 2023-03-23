#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	area_text = new wxStaticText(this, wxID_ANY, wxString("Area"));
	area_value = NULL;

	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizerAndFit(sizer);
	sizer->Add(area_text, wxSizerFlags().Expand().Proportion(50));
	sizer->Add(area_value, wxSizerFlags().Expand().Proportion(50));
}

void iconic::SidePanel::SetAreaText(wxString text) {
	area_value = new wxStaticText(this, wxID_ANY, text);
}

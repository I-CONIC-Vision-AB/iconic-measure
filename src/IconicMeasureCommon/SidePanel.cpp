#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>
#include <IconicMeasureCommon/Shape.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxScrolled<wxPanel>(parent, wxID_ANY) {
	SetScrollRate(0, FromDIP(10));
	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);
}

void iconic::SidePanel::Update(std::vector <boost::shared_ptr<iconic::Shape>> shapes) {
	this->Freeze();
	for (boost::shared_ptr<iconic::Shape> shape : shapes) {
		CreatePanel(*shape);
	}
	this->Thaw();
	FitInside();
}

void iconic::SidePanel::CreatePanel(Shape& shape) {
	switch (shape.GetType()) {
	case iconic::ShapeType::PointType:
		CreatePointPanel(shape);
		break;
	case iconic::ShapeType::LineType:
		CreateLinePanel(shape);
		break;
	case iconic::ShapeType::PolygonType:
		CreatePolygonPanel(shape);
		break;
	}
}


void iconic::SidePanel::CreatePointPanel(Shape& shape) {
	if (shape.HasPanel()) {
		return;
	}

	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	panel->SetBackgroundColour(shape.GetColor());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Point");
	sizer->Add(label, 0, wxEXPAND | wxALL, 10);

	panel->SetSizerAndFit(sizer);
	this->GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);

	shape.SetPanel(panel);
}

void iconic::SidePanel::CreateLinePanel(Shape& shape) {
	if (shape.HasPanel()) {
		wxWindow::FindWindowByName(wxString("length_value"), shape.GetPanel())->SetLabel(wxString(std::to_string(shape.GetLength())));
		return;
	}

	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	panel->SetBackgroundColour(shape.GetColor());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	//Label text
	wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxString("Line"));

	//Length panel and sizer
	wxSizer* length_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* length_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* length_label = new wxStaticText(length_panel, wxID_ANY, wxString("Length "));
	wxStaticText* length_value = new wxStaticText(length_panel, wxID_ANY, wxString(std::to_string((shape.GetLength()))), wxDefaultPosition, wxDefaultSize, 0L, wxString("length_value"));

	length_sizer->Add(length_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	length_sizer->Add(length_value, 0, wxALIGN_CENTER_VERTICAL);
	length_panel->SetSizerAndFit(length_sizer);

	sizer->Add(label, 0, wxEXPAND | wxALL, 10);
	sizer->Add(length_panel, 0, wxEXPAND | wxALL, 10);

	panel->SetSizerAndFit(sizer);
	this->GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);

	shape.SetPanel(panel);
}

void iconic::SidePanel::CreatePolygonPanel(Shape& shape) {
	if (shape.HasPanel()) {
		wxWindow::FindWindowByName(wxString("length_value"), shape.GetPanel())->SetLabel(wxString(std::to_string(shape.GetLength())));
		wxWindow::FindWindowByName(wxString("area_value"), shape.GetPanel())->SetLabel(wxString(std::to_string(shape.GetArea())));
		return;
	}

	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetBackgroundColour(shape.GetColor());

	wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxString("Polygon"));

	wxSizer* length_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* length_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* length_label = new wxStaticText(length_panel, wxID_ANY, wxString("Length "));
	wxStaticText* length_value = new wxStaticText(length_panel, wxID_ANY, wxString(std::to_string((shape.GetLength()))), wxDefaultPosition, wxDefaultSize, 0L, wxString("length_value"));

	length_sizer->Add(length_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	length_sizer->Add(length_value, 0, wxALIGN_CENTER_VERTICAL);
	length_panel->SetSizerAndFit(length_sizer);

	wxSizer* area_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* area_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* area_label = new wxStaticText(area_panel, wxID_ANY, wxString("Area "));
	wxStaticText* area_value = new wxStaticText(area_panel, wxID_ANY, wxString(std::to_string((shape.GetArea()))), wxDefaultPosition, wxDefaultSize, 0L, wxString("area_value"));
	area_sizer->Add(area_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	area_sizer->Add(area_value, 0, wxALIGN_CENTER_VERTICAL);
	area_panel->SetSizerAndFit(area_sizer);

	sizer->Add(label, 0, wxEXPAND | wxALL, 10);
	sizer->Add(length_panel, 0, wxEXPAND | wxALL, 10);
	sizer->Add(area_panel, 0, wxEXPAND | wxALL, 10);

	panel->SetSizerAndFit(sizer);
	this->GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);
	shape.SetPanel(panel);
}
#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>
#include <IconicMeasureCommon/Shape.h>
#include <IconicMeasureCommon/DataUpdateEvent.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxScrolled<wxPanel>(parent, wxID_ANY) {
	SetScrollRate(0, FromDIP(10));
	cSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(cSizer);
}
SidePanel::~SidePanel() {
	for (wxPanel* p : cvPanels) {
		p->Destroy();
	}
}

void SidePanel::Update(DataUpdateEvent& e) {
	Freeze();
	ShapePtr shape = e.GetShape();
	if (e.IsDeletionEvent()) {
		if (e.GetIndex() == -1) {
			for (wxPanel* p : cvPanels) {
				p->Destroy();
			}
			cvPanels.clear();
		} else {
			cvPanels.at(e.GetIndex())->Destroy();
			cvPanels.erase(cvPanels.begin() + e.GetIndex());
		}
	} else if (!(e.GetIndex() < cvPanels.size())) {
		CreatePanel(shape);
	} else {
		switch (shape->GetType()) {
		case iconic::ShapeType::PointType:
			UpdatePointPanel(cvPanels.at(e.GetIndex()), shape);
			break;
		case iconic::ShapeType::LineType:
			UpdateLinePanel(cvPanels.at(e.GetIndex()), shape);
			break;
		case iconic::ShapeType::PolygonType:
			UpdatePolygonPanel(cvPanels.at(e.GetIndex()), shape);
			break;
		}
	}
	Thaw();
	FitInside();
	e.Skip(); // Ensures that other handlers gets the event
}

void SidePanel::UpdatePointPanel(wxPanel* panel, ShapePtr shape) {
	Geometry::Point3D p;
	shape->GetCoordinate(p);
	wxWindow::FindWindowByName(wxString("x_value"), panel)->SetLabel(wxString(std::to_string(p.get<0>())));
	wxWindow::FindWindowByName(wxString("y_value"), panel)->SetLabel(wxString(std::to_string(p.get<1>())));
	wxWindow::FindWindowByName(wxString("z_value"), panel)->SetLabel(wxString(std::to_string(p.get<2>())));
}

void SidePanel::UpdateLinePanel(wxPanel* panel, ShapePtr shape) {
	wxWindow::FindWindowByName(wxString("length_value"), panel)->SetLabel(wxString(std::to_string(shape->GetLength())));
}

void SidePanel::UpdatePolygonPanel(wxPanel* panel, ShapePtr shape) {
	wxWindow::FindWindowByName(wxString("length_value"), panel)->SetLabel(wxString(std::to_string(shape->GetLength())));
	wxWindow::FindWindowByName(wxString("area_value"), panel)->SetLabel(wxString(std::to_string(shape->GetArea())));
	wxWindow::FindWindowByName(wxString("volume_value"), panel)->SetLabel(wxString(std::to_string(shape->GetVolume())));
}

void SidePanel::CreatePanel(ShapePtr shape) {
	switch (shape->GetType()) {
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


void iconic::SidePanel::CreatePointPanel(ShapePtr shape) {
	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	panel->SetBackgroundColour(shape->GetColor());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// show info about the point
	wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxString("Point"));
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	label->SetFont(font);

	Geometry::Point3D p;
	shape->GetCoordinate(p);

	// x coordinate panel and sizer
	wxSizer* x_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* x_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* x_label = new wxStaticText(x_panel, wxID_ANY, wxString("x: "));
	wxStaticText* x_value = new wxStaticText(x_panel, wxID_ANY, wxString(std::to_string(p.get<0>())), wxDefaultPosition, wxDefaultSize, 0L, wxString("x_value"));
	x_sizer->Add(x_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	x_sizer->Add(x_value, 0, wxALIGN_CENTER_VERTICAL);
	x_panel->SetSizerAndFit(x_sizer);

	// y coordinate panel and sizer
	wxSizer* y_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* y_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* y_label = new wxStaticText(y_panel, wxID_ANY, wxString("y: "));
	wxStaticText* y_value = new wxStaticText(y_panel, wxID_ANY, wxString(std::to_string(p.get<1>())), wxDefaultPosition, wxDefaultSize, 0L, wxString("y_value"));
	y_sizer->Add(y_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	y_sizer->Add(y_value, 0, wxALIGN_CENTER_VERTICAL);
	y_panel->SetSizerAndFit(y_sizer);

	// z coordinate panel and sizer
	wxSizer* z_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* z_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* z_label = new wxStaticText(z_panel, wxID_ANY, wxString("z: "));
	wxStaticText* z_value = new wxStaticText(z_panel, wxID_ANY, wxString(std::to_string(p.get<2>())), wxDefaultPosition, wxDefaultSize, 0L, wxString("z_value"));
	z_sizer->Add(z_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	z_sizer->Add(z_value, 0, wxALIGN_CENTER_VERTICAL);
	z_panel->SetSizerAndFit(z_sizer);

	sizer->Add(label, 0, wxEXPAND | wxALL, 10);
	sizer->Add(x_panel, 0, wxEXPAND | wxALL, 10);
	sizer->Add(y_panel, 0, wxEXPAND | wxALL, 10);
	sizer->Add(z_panel, 0, wxEXPAND | wxALL, 10);


	panel->SetSizerAndFit(sizer);
	GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);

	cvPanels.push_back(panel);
}

void iconic::SidePanel::CreateLinePanel(ShapePtr shape) {
	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	panel->SetBackgroundColour(shape->GetColor());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	//Label text
	wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxString("Line"));
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	label->SetFont(font);

	//Length panel and sizer
	wxSizer* length_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* length_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* length_label = new wxStaticText(length_panel, wxID_ANY, wxString("Length: "));
	wxStaticText* length_value = new wxStaticText(length_panel, wxID_ANY, wxString(std::to_string(shape->GetLength())), wxDefaultPosition, wxDefaultSize, 0L, wxString("length_value"));

	length_sizer->Add(length_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	length_sizer->Add(length_value, 0, wxALIGN_CENTER_VERTICAL);
	length_panel->SetSizerAndFit(length_sizer);

	sizer->Add(label, 0, wxEXPAND | wxALL, 10);
	sizer->Add(length_panel, 0, wxEXPAND | wxALL, 10);

	panel->SetSizerAndFit(sizer);
	GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);

	cvPanels.push_back(panel);
}

void iconic::SidePanel::CreatePolygonPanel(ShapePtr shape) {
	// Parent window is the sidepanel
	wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetBackgroundColour(shape->GetColor());

	// create bold font
	wxStaticText* label = new wxStaticText(panel, wxID_ANY, wxString("Polygon"));
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	label->SetFont(font);

	wxSizer* length_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* length_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* length_label = new wxStaticText(length_panel, wxID_ANY, wxString("Perimeter: "));
	wxStaticText* length_value = new wxStaticText(length_panel, wxID_ANY, wxString(std::to_string(shape->GetLength())), wxDefaultPosition, wxDefaultSize, 0L, wxString("length_value"));

	length_sizer->Add(length_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	length_sizer->Add(length_value, 0, wxALIGN_CENTER_VERTICAL);
	length_panel->SetSizerAndFit(length_sizer);

	wxSizer* area_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* area_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* area_label = new wxStaticText(area_panel, wxID_ANY, wxString("Area: "));
	wxStaticText* area_value = new wxStaticText(area_panel, wxID_ANY, wxString(std::to_string(shape->GetArea())), wxDefaultPosition, wxDefaultSize, 0L, wxString("area_value"));
	area_sizer->Add(area_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	area_sizer->Add(area_value, 0, wxALIGN_CENTER_VERTICAL);
	area_panel->SetSizerAndFit(area_sizer);

	wxSizer* volume_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel* volume_panel = new wxPanel(panel, wxID_ANY);
	wxStaticText* volume_label = new wxStaticText(volume_panel, wxID_ANY, wxString("Volume: "));
	wxStaticText* volume_value = new wxStaticText(volume_panel, wxID_ANY, wxString(std::to_string(shape->GetVolume())), wxDefaultPosition, wxDefaultSize, 0L, wxString("volume_value"));
	volume_sizer->Add(volume_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
	volume_sizer->Add(volume_value, 0, wxALIGN_CENTER_VERTICAL);
	volume_panel->SetSizerAndFit(volume_sizer);

	sizer->Add(label, 0, wxEXPAND | wxALL, 10);
	sizer->Add(length_panel, 0, wxEXPAND | wxALL, 10);
	sizer->Add(area_panel, 0, wxEXPAND | wxALL, 10);
	sizer->Add(volume_panel, 0, wxEXPAND | wxALL, 10);

	panel->SetSizerAndFit(sizer);
	GetSizer()->Add(panel, 0, wxEXPAND | wxALL, 10);
	cvPanels.push_back(panel);
}
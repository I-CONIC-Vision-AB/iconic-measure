#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	shapes = NULL;
	sizer = new wxBoxSizer(wxVERTICAL);
	this->SetBackgroundColour(wxColour(255, 255, 255));
	this->SetSizerAndFit(sizer);
}

void iconic::SidePanel::SetShapesPtr(std::vector <boost::shared_ptr<iconic::Geometry::Shape>>* shapes) {
	this->shapes = shapes;
}

void iconic::SidePanel::Update() {
	this->Freeze();
	this->DestroyChildren();
	
	sizer->Clear();
	sizer->Layout();

	if (!shapes) {
		return;
	}

	panels.clear();

	for (boost::shared_ptr<iconic::Geometry::Shape> shape : *(shapes)) {
		// Create outer holder for shape
		wxPanel* shape_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
		wxSizer* shape_sizer = new wxBoxSizer(wxVERTICAL);

		shape_panel->SetBackgroundColour(shape->GetColor());

		wxPanel* area_panel = new wxPanel(shape_panel, wxID_ANY);
		wxSizer* area_sizer = new wxBoxSizer(wxHORIZONTAL);

		// Create holder for area label and value
		wxStaticText* area_text = new wxStaticText(area_panel, wxID_ANY, wxString("Area"), wxDefaultPosition, wxDefaultSize);

		double area = shape->GetArea();//boost::geometry::area(*(shape->renderCoordinates));
		wxStaticText* area_value = new wxStaticText(area_panel, wxID_ANY, wxString(std::to_string(area)), wxDefaultPosition, wxDefaultSize);

		area_sizer->Add(area_text, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
		area_sizer->Add(area_value, 0, wxALIGN_CENTER_VERTICAL);

		// Create holder for length label and text
		wxPanel* length_panel = new wxPanel(shape_panel, wxID_ANY);
		wxSizer* length_sizer = new wxBoxSizer(wxHORIZONTAL);

		wxStaticText* length_text = new wxStaticText(length_panel, wxID_ANY, wxString("Length"), wxDefaultPosition, wxDefaultSize);

		double length = shape->GetLength();//boost::geometry::length(*(shape->renderCoordinates));
		wxStaticText* length_value = new wxStaticText(length_panel, wxID_ANY, wxString(std::to_string(length)), wxDefaultPosition, wxDefaultSize);

		length_sizer->Add(length_text, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
		length_sizer->Add(length_value, 0, wxALIGN_CENTER_VERTICAL);

		// Add the now made panel to the panels to be rendered
		panels.push_back(shape_panel);

		// Fix sizers
		area_panel->SetSizerAndFit(area_sizer);
		length_panel->SetSizerAndFit(length_sizer);

		shape_sizer->Add(area_panel, 1, wxEXPAND | wxTOP | wxLEFT | wxBOTTOM, 20);
		shape_sizer->Add(length_panel, 1, wxEXPAND | wxBOTTOM | wxLEFT, 20);

		shape_panel->SetSizerAndFit(shape_sizer);
	}

	for (wxPanel* panel : panels) {
		// Add it to the main (SidePanel) sizer
		sizer->Add(panel, 0, wxEXPAND | wxALL, 20);
	}

	sizer->Layout();
	this->Thaw();
	this->Fit();
}

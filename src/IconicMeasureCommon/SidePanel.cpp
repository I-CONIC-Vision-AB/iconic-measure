#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	sizer = new wxBoxSizer(wxVERTICAL);
}

void iconic::SidePanel::Update(std::vector <boost::shared_ptr<iconic::Geometry::Shape>> shapes) {
	this->Thaw();
	for (boost::shared_ptr<iconic::Geometry::Shape> shape : shapes) {
		//shape->CreatePanel();
		
		wxPanel* panel = shape->GetPanelPointer();

		panel->Reparent(this);
		panel->Thaw();
		panel->SetSize(wxSize(200, 200));
		panel->SetBackgroundColour(wxColour(100, 255, 255));

		wxStaticText* test = new wxStaticText(panel, wxID_ANY, wxString("What the FUck"), wxDefaultPosition, wxSize(100, 100));
		test->SetBackgroundColour(wxColour(255, 100, 255));
		panel->GetSizer()->Add(test, 1, wxEXPAND | wxALL, 20);

		if (panel == nullptr) {
			wxLogVerbose("panel not created");
			return;
		}

		wxString labelText;
		// Iterate through the children of the panel to find the wxStaticText control
		for (wxWindowList::Node* node = panel->GetChildren().GetFirst(); node; node = node->GetNext()) {
			wxWindow* child = node->GetData();
			wxStaticText* aStaticText = dynamic_cast<wxStaticText*>(child);
			wxLogError(_("SIZE OF CHILDREN: " + std::to_string(child->GetChildren().GetCount())));
			if (aStaticText) {
				labelText = aStaticText->GetLabel();
				wxLogError(wxString("LABEL: " + labelText));
			}


		}

		wxLogVerbose("tried to render something");
		
		sizer->Add(panel, 1, wxEXPAND | wxALL, 50);

		panel->Show();
	}

	this->SetSizerAndFit(sizer);
}

/*
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

		shape_panel->SetBackgroundColour(wxColor(shape->GetColor().red, shape->GetColor().blue, shape->GetColor().green));

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
*/

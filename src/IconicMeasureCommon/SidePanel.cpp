#include <IconicMeasureCommon/SidePanel.h>
#include <wx/wx.h>

using namespace iconic;

iconic::SidePanel::SidePanel(wxWindow* parent) : wxScrolled<wxPanel>(parent, wxID_ANY) {
	SetScrollRate(0, FromDIP(10));
	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);
}

void iconic::SidePanel::Update(std::vector <boost::shared_ptr<iconic::Geometry::Shape>> shapes) {
	this->Freeze();
	for (boost::shared_ptr<iconic::Geometry::Shape> shape : shapes) {
		shape->CreatePanel(this);
	}
	this->Thaw();
	FitInside();
}

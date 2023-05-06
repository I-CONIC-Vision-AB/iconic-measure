#pragma once
#include <wx/wx.h>
#include <IconicMeasureCommon/exports.h>
#include <boost/shared_ptr.hpp>
#include <IconicMeasureCommon/Geometry.h>
#include <IconicMeasureCommon/DataUpdateEvent.h>
#include <vector>


namespace iconic {
	class ICONIC_MEASURE_COMMON_EXPORT SidePanel : public wxScrolled<wxPanel> {

	public:
		SidePanel(wxWindow* parent);
		~SidePanel();
		void Update(DataUpdateEvent& e);
		

	private:
		void CreatePanel(DataUpdateEvent& e);
		void CreatePointPanel(DataUpdateEvent& e);
		void CreateLinePanel(DataUpdateEvent& e);
		void CreatePolygonPanel(DataUpdateEvent& e);
		void UpdatePointPanel(wxPanel*, DataUpdateEvent& e);
		void UpdateLinePanel(wxPanel*, DataUpdateEvent& e);
		void UpdatePolygonPanel(wxPanel*, DataUpdateEvent& e);

		wxBoxSizer* cSizer;
		std::vector<wxPanel*> cvPanels;
	};
}
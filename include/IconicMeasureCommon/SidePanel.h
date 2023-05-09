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
		/**
		 * @brief Constructor
		 * @param parent Window the sidepanel belongs to. Should be the VideoPlayerFrame
		*/
		SidePanel(wxWindow* parent);
		/**
		 * @brief Destructor. Destroys panels
		*/
		~SidePanel();
		/**
		 * @brief Eventhandler for the DataUpdateEvent
		 * @param e The event data
		*/
		void Update(DataUpdateEvent& e);


	private:
		/**
		 * @brief Intermediary method for creating new panel
		 * @param e Event data
		*/
		void CreatePanel(ShapePtr shape);
		/**
		 * @brief Method for creating a point panel
		 * @param e Event data
		*/
		void CreatePointPanel(ShapePtr shape);
		/**
		 * @brief Method for creating a line panel
		 * @param e Event data
		*/
		void CreateLinePanel(ShapePtr shape);
		/**
		 * @brief Method for creating a polygon panel
		 * @param e Event data
		*/
		void CreatePolygonPanel(ShapePtr shape);
		/**
		 * @brief Method for updating a point panel
		 * @param panel The panel to update
		 * @param e Event data
		*/
		void UpdatePointPanel(wxPanel* panel, ShapePtr shape);
		/**
		* @brief Method for updating a line panel
		* @param panel The panel to update
		* @param e Event data
		*/
		void UpdateLinePanel(wxPanel* panel, ShapePtr shape);
		/**
		* @brief Method for updating a polygon panel
		* @param panel The panel to update
		* @param e Event data
		*/
		void UpdatePolygonPanel(wxPanel* panel, ShapePtr shape);

		wxBoxSizer* cSizer;
		std::vector<wxPanel*> cvPanels;
	};
}
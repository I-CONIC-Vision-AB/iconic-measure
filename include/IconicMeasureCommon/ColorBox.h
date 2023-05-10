#pragma once
#include <IconicMeasureCommon/exports.h>
#include <wx/glcanvas.h>
#include <wx/generic/filectrlg.h>

namespace iconic {
	/**
	 * @brief Displays the color of the selected shape in the toolbar data display.
	*/
	class ICONIC_MEASURE_COMMON_EXPORT ColorBox : public wxControl
	{
	public:
		/**
		 * @brief Constructor
		 * @param parent The parent window. Should be the toolbar
		 * @param id The ID of the colorbox
		 * @param pos The position of the colorbox
		 * @param size The size of the colorbox
		 * @param style The style of the colorbox
		*/
		ColorBox(wxWindow* parent, wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxBORDER_NONE);

		/**
		 * @brief Sets the color of the colorbox
		 * @param color The color to set the colorbox to
		*/
		void SetColor(const wxColour& color);

	private:
		wxColour m_color; //!< The color of the colorbox

		/**
		 * @brief The paint event. Makus sure that the displayed color is accurate
		 * @param event unused
		*/
		void OnPaint(wxPaintEvent& event);

		wxDECLARE_EVENT_TABLE();
	};
}
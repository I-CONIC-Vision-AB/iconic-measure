#include	<IconicMeasureCommon/IconicMeasureHandler.h>

IconicMeasureHandler::IconicMeasureHandler()
{

}

bool IconicMeasureHandler::OnNextFrame(iconic::gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse) {
	return true;
}

wxString IconicMeasureHandler::GetMetaFileName() const {
	return cMetaFileName;
}

bool IconicMeasureHandler::Parse() { 
	return true; 
}

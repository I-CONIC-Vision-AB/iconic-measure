#pragma once
#include <IconicMeasureCommon/exports.h>
#include <IconicGpu/MetaDataHandler.h>

class ICONIC_MEASURE_COMMON_EXPORT IconicMeasureHandler : public iconic::gpu::MetaDataHandler {
public:

	IconicMeasureHandler();

    /**
 * @brief Called for each frame/image
 * @param filename Meta data file name
 * @param frameNumber Frame or image number
 * @param time Time stamp
 * @param imSize Image size
 * @param bDoParse Call \c Parse (e.g. to read image transformation). If only saving data while processing, parsing may be unnecessary.
 * @return True on success
 * @sa Parse
*/
    virtual bool OnNextFrame(iconic::gpu::ImagePropertyPtr pProperties, wxString const& filename, int const& frameNumber, float const& time, boost::compute::uint2_ const& imSize, bool bDoParse = false);

    virtual wxString GetMetaFileName() const;

    /**
     * @brief Read the meta data file.
     *
     * May for example create an image transformation
     * @return True on success
    */
    virtual bool Parse();

private:
    wxString cMetaFileName;
};

typedef boost::shared_ptr<IconicMeasureHandler> IconicMeasureHandlerPtr;
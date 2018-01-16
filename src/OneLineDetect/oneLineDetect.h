#ifndef _oneLineDetect_H_
#define _oneLineDetect_H_

#include "stdafx.h"

#define OID_ADTF_OneLineDetect_FILTER "adtf.example.oneLineDetect"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cOneLineDetect : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_OneLineDetect_FILTER, "oneLineDetect", adtf::OBJCAT_DataFilter);

private:
    int whiteAreaInRow(int targetRow, Mat src, Mat greyImg);
    MedianFilter medianFilter;

protected:
    cVideoPin m_oVideoInputPin;
    cVideoPin m_oVideoOutputPin;
    cOutputPin m_oDiff_CenterPin;

public:
    cOneLineDetect(const tChar* __info);
    virtual ~cOneLineDetect();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


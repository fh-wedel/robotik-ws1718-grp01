#ifndef _medianFilter_H_
#define _medianFilter_H_

#define OID_ADTF_NEGATIVNUMBER_FILTER "adtf.example.negativNumberFilter"

#include "../../helperFunctions.h"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cNegativNumberFilter : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_NEGATIVNUMBER_FILTER, "negativNumberFilter", adtf::OBJCAT_DataFilter);

protected:
    cInputPin m_oInputPin;
    cOutputPin m_oOutputPin;

public:
    cNegativNumberFilter(const tChar* __info);
    virtual ~cNegativNumberFilter();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


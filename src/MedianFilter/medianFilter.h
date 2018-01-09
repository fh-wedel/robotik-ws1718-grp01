#ifndef _medianFilter_H_
#define _medianFilter_H_

#define OID_ADTF_MEDIAN_FILTER "adtf.example.medianFilter"

#include "../../protocol.h"
#include "../../helperFunctions.h"
#include "../Helper/helper.h"
#include <bits/unique_ptr.h>

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cMedianFilter : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_MEDIAN_FILTER, "medianFilter", adtf::OBJCAT_DataFilter);

private:
    //Listen zur Speicherung der Messwerte
    //vector<FilterValue> _list;
    unique_ptr<MedianFilter> medianFilter;

protected:
    cInputPin m_oInputPin;
    cOutputPin m_oOutputPin;

public:
    cMedianFilter(const tChar* __info);
    virtual ~cMedianFilter();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


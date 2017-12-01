#ifndef _TESTBILDFILTER_H_
#define _TESTBILDFILTER_H_

#include "stdafx.h"
#include "ADTF_OpenCV_helper.h"

#include <aadc_structs.h>
#include <iostream>
#include "../../protocol.h"

#define OID_ADTF_TESTBILDFILTER "adtf.example.testBildFilter"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cTestBildFilter : public adtf::cFilter {
    ADTF_FILTER(OID_ADTF_TESTBILDFILTER, "testBildFilter", adtf::OBJCAT_DataFilter);

private:
    Mat _saveImage;

protected:
    cInputPin m_oInputPin;
    cOutputPin m_oOutputPin;

public:
    cTestBildFilter(const tChar* __info);
    virtual ~cTestBildFilter();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _TESTBILDFILTER_H_


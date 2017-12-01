#ifndef _TESTBILDFILTER_H_
#define _TESTBILDFILTER_H_

#include "stdafx.h"
#include "ADTF_OpenCV_helper.h"

#include <aadc_structs.h>
#include <iostream>
#include "../../protocol.h"

#define OID_ADTF_TESTBILDFILTER "adtf.example.testBildFilter"

class cTestBildFilter : public adtf::cFilter {
    ADTF_FILTER(OID_ADTF_TESTBILDFILTER, "testBildFilter", adtf::OBJCAT_DataFilter);

private:

protected:
    cVideoPin m_oVideoInputPin;
    cVideoPin m_oVideoOutputPin;

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


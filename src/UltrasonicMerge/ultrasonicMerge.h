#ifndef _ultrasonicMerge_H_
#define _ultrasonicMerge_H_

#define OID_ADTF_ULTRASONICMERGE "adtf.example.ultrasonicMerge"

#include "../../protocol.h"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cUltrasonicMerge : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_ULTRASONICMERGE, "ultrasonicMerge", adtf::OBJCAT_DataFilter);

private:
    UltrasonicStruct _ultrasonicStruct;
    uint32_t _bitTest;

protected:
    cInputPin m_oInputPinFrontCenter;
    cInputPin m_oInputPinFrontCenterLeft;
    cInputPin m_oInputPinFrontLeft;
    cInputPin m_oInputPinSideLeft;
    cInputPin m_oInputPinRearLeft;
    cInputPin m_oInputPinRearCenter;
    cInputPin m_oInputPinSideRight;
    cInputPin m_oInputPinRearRight;
    cInputPin m_oInputPinFrontRight;
    cInputPin m_oInputPinFrontCenterRight;

    cOutputPin m_oOutputPin;


public:
    cUltrasonicMerge(const tChar* __info);
    virtual ~cUltrasonicMerge();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


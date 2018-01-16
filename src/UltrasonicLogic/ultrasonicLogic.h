#ifndef _ULTRASONICLOGIC_H_
#define _ULTRASONICLOGIC_H_

#include "../../helperFunctions.h"

#define OID_ADTF_TEMPLATE_FILTER "adtf.example.ultrasonicLogic"


class cUltrasonicLogic : public adtf::cFilter
{
    ADTF_FILTER(OID_ADTF_TEMPLATE_FILTER, "UltrasonicLogic", adtf::OBJCAT_DataFilter);
private:
    UltrasonicStruct _ultrasonicStruct;
    MedianFilter frontCenterFilter;
    MedianFilter frontCenterLeftFilter;
    MedianFilter frontLeftFilter;
    MedianFilter sideLeftFilter;
    MedianFilter rearLeftFilter;
    MedianFilter rearCenterFilter;
    MedianFilter rearRightFilter;
    MedianFilter sideRightFilter;
    MedianFilter frontRightFilter;
    MedianFilter frontCenterRightFilter;

protected:
    cInputPin m_oInputPin;
    cOutputPin m_oOutputPin;
    cOutputPin m_oOutputEmergencyFlagPin;

public:
    cUltrasonicLogic(const tChar* __info);
    virtual ~cUltrasonicLogic();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

#endif // _ULTRASONICLOGIC_H_


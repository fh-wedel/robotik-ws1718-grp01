
#ifndef _TEMPLATE_FILTER_H_
#define _TEMPLATE_FILTER_H_

#define OID_ADTF_CARCONTROLLER "adtf.example.controller2"

#include "../../protocol.h"

#define CALIBRATION 1
#define CALIBRATION_SAMPLES 60


#define G 9.80665
#define ACC_Z_NORMAL 9.75
#define ACC_Z_DELTA_NEG 0.02
#define ACC_Z_DELTA_POS 0.01
#define  MAX_SPEED -12


#define ACC_Y_NORMAL -0.85
#define ACC_X_NORMAL 0.13


class c_controller : public adtf::cFilter
{
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_CARCONTROLLER, "Contoller", adtf::OBJCAT_DataFilter);

private:
    float getSmallerSpeed(float oldVal, float newVal);
    //cObjectPtr<IMediaTypeDescription> m_pDescriptionAccelerateSignalInput;
    MotorControl _motorControl;

protected:
    cInputPin    m_oInputPin_USS;
    cInputPin    m_oInputPin_diff;
    //cInputPin    m_oInputPin_speed;
    cOutputPin    m_oOutputPin_carcontrol;

public:
    c_controller(const tChar* __info);
    virtual ~c_controller();

protected:
    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);
    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);
    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);

};

#endif


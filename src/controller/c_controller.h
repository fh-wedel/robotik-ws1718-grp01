
#ifndef _TEMPLATE_FILTER_H_
#define _TEMPLATE_FILTER_H_

#define OID_ADTF_CARCONTROLLER "adtf.example.controller2"

#include "../../protocol.h"



//todo fuer morgen... KalmanFilter implementieren fuer Lenkung und Ultrasonic
//todo fuer morgen... Dynamischer BildFilter (Kontrastanpassung - Blauanteil)

typedef struct SensorAngle {
    float REAR_CENTER_ANGLE;
    float REAR_LEFT_ANGLE;
    float SIDE_LEFT_ANGLE;
    float FRONT_LEFT_ANGLE;
    float FRONT_CENTER_LEFT_ANGLE;
    float FRONT_CENTER_ANGLE;
    float FRONT_CENTER_RIGHT_ANGLE;
    float FRONT_RIGHT_ANGLE;
    float SIDE_RIGHT_ANGLE;
    float REAR_RIGHT_ANGLE;

    SensorAngle() {
        REAR_CENTER_ANGLE = 1;
        REAR_LEFT_ANGLE = -1;
        SIDE_LEFT_ANGLE = -0.7f;
        FRONT_LEFT_ANGLE = -0.4f;
        FRONT_CENTER_LEFT_ANGLE = -0.2f;
        FRONT_CENTER_ANGLE = 0;
        FRONT_CENTER_RIGHT_ANGLE = 0.2f;
        FRONT_RIGHT_ANGLE = 0.4f;
        SIDE_RIGHT_ANGLE = 0.7f;
        REAR_RIGHT_ANGLE = 1;
    }
} SensorAngle;

class c_controller : public adtf::cFilter
{
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_CARCONTROLLER, "Contoller", adtf::OBJCAT_DataFilter);

private:
    MotorControl _motorControl;
    SensorAngle sa;


protected:
    cInputPin    m_oInputPin_USS;
    cInputPin    m_oInputPin_diff;
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


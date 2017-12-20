
#include "stdafx.h"
#include "c_motorcontrol.h"
#include "../../protocol.h"
#include <stdio.h>
#include <cmath>



ADTF_FILTER_PLUGIN("Motorcontrol", OID_ADTF_TEMPLATE_FILTER, c_motorcontrol);

float cur_speed = 0;
float cur_angle = 0;
bool emergeny_break_enabled = 0;
int intital0send = 0;


c_motorcontrol::c_motorcontrol(const tChar *__info) : cFilter(__info) {

}

c_motorcontrol::~c_motorcontrol() {

}


tResult c_motorcontrol::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        cObjectPtr<IMediaType> pMotorcontrolType;
        RETURN_IF_FAILED(AllocMediaType(&pMotorcontrolType, MEDIA_TYPE_MOTORCONTROL, MEDIA_SUBTYPE_MOTORCONTROL, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_motorcontol.Create("Motorcontrol", pMotorcontrolType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_motorcontol));

        cObjectPtr<IMediaType> pFlagType;
        RETURN_IF_FAILED(AllocMediaType(&pFlagType, MEDIA_TYPE_FLAG, MEDIA_SUBTYPE_FLAG, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_flags.Create("flag_in", pFlagType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_flags));

        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin_speed.Create("speed_out", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_speed));
        RETURN_IF_FAILED(m_oOutputPin_angle.Create("angle_out", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_angle));
    }
    RETURN_NOERROR;
}

tResult c_motorcontrol::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}


tResult c_motorcontrol::OnPinEvent(IPin *pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample *pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oInputPin_flags) {

            Flag flags = receiveData<Flag>(pMediaSample);
            if (flags & FLAG_EMERGENCY_BREAK) {
                emergeny_break();
            }
            if (flags == 2) {
                emergeny_break_enabled = 0;
            }
        } else if (pSource == &m_oInputPin_motorcontol) {
			
			if (intital0send < 150) {
			
                ArduinoTransmitFloatValue(&m_oOutputPin_speed, 0.0, 0);
                intital0send++;
			
			} else {

                MotorControl motorControl_data = receiveData<MotorControl>(pMediaSample);
                float new_speed = motorControl_data.speed;
                float new_angle = motorControl_data.angle;

                if (!emergeny_break_enabled) {
                    ArduinoTransmitFloatValue(&m_oOutputPin_speed, new_speed, 0);
                }
                ArduinoTransmitFloatValue(&m_oOutputPin_angle, new_angle, 0);
            }
        }
    }

    RETURN_NOERROR;
}


void c_motorcontrol::emergeny_break() {
    cur_speed = 0;
    emergeny_break_enabled = 1;
    printf("emergency break enabled");
    ArduinoTransmitFloatValue(&m_oOutputPin_speed, cur_speed, 0);
    
}

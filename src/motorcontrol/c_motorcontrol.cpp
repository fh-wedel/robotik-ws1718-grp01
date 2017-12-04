/**
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2017-05-12 09:34:53#$ $Rev:: 63109   $
**********************************************************************/


#include "stdafx.h"
#include "c_motorcontrol.h"
#include "../../protocol.h"
#include <stdio.h>
#include <cmath>



/// Create filter shell
ADTF_FILTER_PLUGIN("Motorcontrol", OID_ADTF_TEMPLATE_FILTER, c_motorcontrol);


tFloat32 sum_x = 0;
tFloat32 sum_y = 0;
tFloat32 sum_z = 0;
tInt32 samples = 0;

float cur_speed = 0;
float cur_angle = 0;
bool emergeny_break_enabled = 0;


c_motorcontrol::c_motorcontrol(const tChar *__info) : cFilter(__info) {

}

c_motorcontrol::~c_motorcontrol() {

}


tResult c_motorcontrol::Init(tInitStage eStage, __exception) {
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    // in StageFirst you can create and register your static pins.
    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pMotorcontrolType;
        RETURN_IF_FAILED(AllocMediaType(&pMotorcontrolType, MEDIA_TYPE_MOTORCONTROL, MEDIA_SUBTYPE_MOTORCONTROL,
                                        __exception_ptr));

        cObjectPtr<IMediaType> pAccelerationlType;
        RETURN_IF_FAILED(AllocMediaType(&pAccelerationlType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER, __exception_ptr));

        cObjectPtr<IMediaType> pFlagType;
        RETURN_IF_FAILED(AllocMediaType(&pFlagType, MEDIA_TYPE_FLAG, MEDIA_SUBTYPE_FLAG, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_motorcontol.Create("Motorcontrol", pMotorcontrolType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_motorcontol));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_acceleration.Create("acc_in", pAccelerationlType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_acceleration));



        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_flags.Create("flag_in", pFlagType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_flags));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin_speed.Create("speed_out", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_speed));
        RETURN_IF_FAILED(m_oOutputPin_angle.Create("angle_out", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_angle));
    } else if (eStage == StageNormal) {
        // In this stage you would do further initialisation and/or create your dynamic pins.
        // Please take a look at the demo_dynamicpin example for further reference.
    } else if (eStage == StageGraphReady) {
        // All pin connections have been established in this stage so you can query your pins
        // about their media types and additional meta data.
        // Please take a look at the demo_imageproc example for further reference.
    }

    RETURN_NOERROR;
}

tResult c_motorcontrol::Shutdown(tInitStage eStage, __exception) {
    // In each stage clean up everything that you initiaized in the corresponding stage during Init.
    // Pins are an exception:
    // - The base class takes care of static pins that are members of this class.
    // - Dynamic pins have to be cleaned up in the ReleasePins method, please see the demo_dynamicpin
    //   example for further reference.

    if (eStage == StageGraphReady) {
    } else if (eStage == StageNormal) {
    } else if (eStage == StageFirst) {
    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}


tResult c_motorcontrol::OnPinEvent(IPin *pSource,
                                   tInt nEventCode,
                                   tInt nParam1,
                                   tInt nParam2,
                                   IMediaSample *pMediaSample) {
    // first check what kind of event it is
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oInputPin_flags) {


            Flag flags = receiveData<Flag>(pMediaSample);
            if (flags & FLAG_EMERGENCY_BREAK) {
				printf("break");
                //emergeny_break();
            }


        }
        if (pSource == &m_oInputPin_motorcontol) {


            MotorControl motorControl_data = receiveData<MotorControl>(pMediaSample);
            float new_speed = motorControl_data.speed;
            float new_angle = motorControl_data.angle;
            if (!emergeny_break_enabled)
                if (cur_speed < new_speed)
                    for (int i = (int) cur_speed; i < (int) new_speed; i++) {
                        float i_float = (float) i;
                        sendData<float>(&m_oOutputPin_speed, &i_float);
                    }
                else
                    for (int i = (int) cur_speed; i > (int) new_speed; i--) {
                        float i_float = (float) i;
                        sendData<float>(&m_oOutputPin_speed, &i_float);
                    }

            if (cur_angle < new_angle)
                for (int i = (int) cur_angle; i < (int) new_angle; i++) {
                    float i_float = (float) i;
                    sendData<float>(&m_oOutputPin_angle, &i_float);
                }
            else
                for (int i = (int) cur_angle; i > (int) new_angle; i--) {
                    float i_float = (float) i;
                    sendData<float>(&m_oOutputPin_angle, &i_float);
                }


            if (!emergeny_break_enabled)
                sendData<float>(&m_oOutputPin_speed, &new_speed);
            sendData<float>(&m_oOutputPin_angle, &new_angle);


        }


        if (pSource == &m_oInputPin_acceleration) {
			printf("%d\n", sizeof(float));
			printf("%d\n", sizeof(tFloat32));
			//tFloat32 test = receiveData<tFloat32>(pMediaSample);
			//sendData<tFloat32>(&m_oOutputPin_speed, &test);

            tInerMeasUnitData inerMeasUnitData = receiveData<tInerMeasUnitData>(pMediaSample);

            tFloat32 acc_x = inerMeasUnitData.f32A_x * G;

            tFloat32 acc_y = inerMeasUnitData.f32A_y * G;

            tFloat32 acc_z = inerMeasUnitData.f32A_z * G;

            if (CALIBRATION) {
                if (samples < CALIBRATION_SAMPLES) {
                    sum_x += acc_x;
                    sum_y += acc_y;
                    sum_z += acc_z;
                    samples++;

                }
                if (samples == CALIBRATION_SAMPLES) {
                    samples++;
                    printf("Nullwerte der Beschleunigungssensoren nach %d samples:\nx:%f\ny:%f\nz:%f\n ",
                           CALIBRATION_SAMPLES, (sum_x / samples), (sum_y / samples), (sum_z / samples));
                }
            }
            //printf("%f\n", acc_z);
            //TODO emergeny break if acceleration(z) is to high
            if (acc_z > ACC_Z_NORMAL + 1.5) {
				printf("%f\n", acc_z);
                emergeny_break();
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

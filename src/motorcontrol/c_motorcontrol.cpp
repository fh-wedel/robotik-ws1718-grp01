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
#include "template_data.h"
/// Create filter shell
ADTF_FILTER_PLUGIN("Motorcontrol", OID_ADTF_TEMPLATE_FILTER, c_motorcontrol);

cCriticalSection m_critSecTransmitControl;

c_motorcontrol::c_motorcontrol(const tChar* __info):cFilter(__info)
{

}

c_motorcontrol::~c_motorcontrol()
{

}



tResult c_motorcontrol::Init(tInitStage eStage, __exception)
{
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    // in StageFirst you can create and register your static pins.
    if (eStage == StageFirst)
    {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_TEMPLATE, MEDIA_SUBTYPE_TEMPLATE, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_speed.Create("speed_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_speed));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_acceleration.Create("acc_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_acceleration));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_angle.Create("angle_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_angle));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_flags.Create("flag_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_flags));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_TEMPLATE, MEDIA_SUBTYPE_TEMPLATE, __exception_ptr));

        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));
        tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
        RETURN_IF_POINTER_NULL(strDescSignalValue);
        cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionFloat));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin_speed.Create("speed_out", pOutputType,static_cast<IPinEventSink*> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_speed));
        RETURN_IF_FAILED(m_oOutputPin_angle.Create("angle_out", pOutputType, static_cast<IPinEventSink*> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_angle));
    }
    else if (eStage == StageNormal)
    {
        // In this stage you would do further initialisation and/or create your dynamic pins.
        // Please take a look at the demo_dynamicpin example for further reference.
    }
    else if (eStage == StageGraphReady)
    {
        // All pin connections have been established in this stage so you can query your pins
        // about their media types and additional meta data.
        // Please take a look at the demo_imageproc example for further reference.
    }

    RETURN_NOERROR;
}

tResult c_motorcontrol::Shutdown(tInitStage eStage, __exception)
{
    // In each stage clean up everything that you initiaized in the corresponding stage during Init.
    // Pins are an exception:
    // - The base class takes care of static pins that are members of this class.
    // - Dynamic pins have to be cleaned up in the ReleasePins method, please see the demo_dynamicpin
    //   example for further reference.

    if (eStage == StageGraphReady)
    {
    }
    else if (eStage == StageNormal)
    {
    }
    else if (eStage == StageFirst)
    {
    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tFloat32 sum_z = 0;
tInt32 samples = 0;
tFloat32 delta_z_neg = 0;
tFloat32 delta_z_pos = 0;

uint32_t unter = 0;
uint32_t ober = 0;

tResult c_motorcontrol::OnPinEvent(IPin* pSource,
                                    tInt nEventCode,
                                    tInt nParam1,
                                    tInt nParam2,
                                    IMediaSample* pMediaSample)
{
    // first check what kind of event it is
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived)
    {
        // so we received a media sample, so this pointer better be valid.
        RETURN_IF_POINTER_NULL(pMediaSample);
        // by comparing it to our member pin variable we can find out which pin received
        // the sample
        if (pSource == &m_oInputPin_acceleration)
        {
            TransmitFloatValue(&m_oOutputPin_speed,10,0);

            tInerMeasUnitData* pSampleData = NULL;
            if (IS_OK(pMediaSample->Lock((const tVoid**)&pSampleData))) {

                tFloat32 acc_x = pSampleData->f32A_x*G;
                acc_x++;
                tFloat32 acc_y = pSampleData->f32A_y*G;
                acc_y++;
                tFloat32 acc_z = pSampleData->f32A_z*G;
                //printf("%f===%f===%f\n", acc_x,acc_y,acc_z);
                if(samples<300) {
                    sum_z += acc_z;
                    samples++;
                    tFloat32 delta = ACC_Z_NORMAL - acc_z;
                    if(delta > 0)
                        delta_z_pos += delta;
                    else
                        delta_z_neg += delta;



                    /*printf("\nsum:%f",sum_z/samples);
                    printf("\ndelta neg:%f",delta_z_neg/samples);
                    printf("\ndelta pos:%f",delta_z_pos/samples);*/
                }


                if ((acc_z > ACC_Z_NORMAL + ACC_Z_DELTA_POS)){
                    /*printf("\nAlarm");
                    printf("o:%d",++ober);*/
                    printf("\n%f", (acc_z - ACC_Z_NORMAL + ACC_Z_DELTA_POS));

                }
                if (acc_z < ACC_Z_NORMAL - ACC_Z_DELTA_NEG){
                    /*printf("\nAlarm");
                    printf("u:%d",++unter);*/
                    printf("\n%f", (acc_z - ACC_Z_NORMAL - ACC_Z_DELTA_NEG));
                }
                //value = pSampleData->tFrontLeft.f32Value;
                //printf("\n");

                pMediaSample->Unlock(pSampleData);
            }



            /*for (int i = 0; i < 100; ++i) {
                TransmitFloatValue(&m_oOutputPin_speed,i,0);
            }
            for (int i = 100; i > 0; --i) {
                TransmitFloatValue(&m_oOutputPin_speed,i,0);
            }*/

        }
    }

    RETURN_NOERROR;
}


tResult c_motorcontrol::TransmitFloatValue(cOutputPin* oPin, tFloat32 value, tUInt32 timestamp)
{
    //use mutex
    __synchronized_obj(m_critSecTransmitControl);

    cObjectPtr<IMediaSample> pMediaSample;
    AllocMediaSample((tVoid**)&pMediaSample);

    cObjectPtr<IMediaSerializer> pSerializer;
    m_pDescriptionFloat->GetMediaSampleName();
    m_pDescriptionFloat->GetMediaSampleSerializer(&pSerializer);
    pMediaSample->AllocBuffer(pSerializer->GetDeserializedSize());

    static bool hasID = false;
    static tBufferID szIDValueOutput;
    static tBufferID szIDArduinoTimestampOutput;

    {
        __adtf_sample_write_lock_mediadescription(m_pDescriptionFloat, pMediaSample, pCoderOutput);

        if(!hasID)
        {
            pCoderOutput->GetID("f32Value", szIDValueOutput);
            pCoderOutput->GetID("ui32ArduinoTimestamp", szIDArduinoTimestampOutput);
            hasID = tTrue;
        }

        pCoderOutput->Set(szIDValueOutput, (tVoid*)&value);
        pCoderOutput->Set(szIDArduinoTimestampOutput, (tVoid*)&timestamp);
    }

    pMediaSample->SetTime(_clock->GetStreamTime());

    oPin->Transmit(pMediaSample);

    RETURN_NOERROR;
}

/*tResult cArduinoCommunication::ProcessActuatorValue(IMediaSample* pMediaSample, tUInt8 ui8ChID)
{
    tFloat32 f32value = 0;

    static bool hasID_SteerServo = false;
    static tBufferID szIDF32Value_SteerServo;

    static bool hasID_SpeedContr = false;
    static tBufferID szIDF32Value_SpeedContr;


    switch (ui8ChID)
    {
        case ID_ARD_ACT_STEER_SERVO:
        {
            __adtf_sample_read_lock_mediadescription(m_pDescriptionSteeringSignalInput, pMediaSample, pCoderInput);

            if (!hasID_SteerServo)
            {
                pCoderInput->GetID("f32Value", szIDF32Value_SteerServo);
                hasID_SteerServo = true;
            }

            pCoderInput->Get(szIDF32Value_SteerServo, (tVoid*)&f32value);

        }
            for (size_t i = 0; i < m_valid_ids.size(); i++)
            {
                if (m_com[i].get_id() == ARDUINO_CENTER_ACTUATORS)
                {
                    m_com[i].send_steering(f32value);
                    break;
                }
            }

            break;
        case ID_ARD_ACT_SPEED_CONTR:
        {
            __adtf_sample_read_lock_mediadescription(m_pDescriptionAccelerateSignalInput, pMediaSample, pCoderInput);

            if (!hasID_SpeedContr)
            {
                pCoderInput->GetID("f32Value", szIDF32Value_SpeedContr);
                hasID_SpeedContr = true;
            }

            pCoderInput->Get(szIDF32Value_SpeedContr, (tVoid*)&f32value);

        }
            for (size_t i = 0; i < m_valid_ids.size(); i++)
            {
                if (m_com[i].get_id() == ARDUINO_CENTER_ACTUATORS)
                {
                    m_com[i].send_speed(f32value);
                    break;
                }
            }

            break;
    }


    RETURN_NOERROR;
}



 void arduino_com_client::send_steering(float angle)
{
    // remap angle
    angle = angle > 100.f ? 100.f : angle;
    angle = angle < -100.f ? -100.f : angle;
    angle += 100.f;
    angle *= 0.9f;

    tArduinoHeader header;
    header.ui8ID = ID_ARD_ACT_STEER_SERVO;
    header.ui8DataLength = sizeof(tServoData);
    header.ui32Timestamp = 0;

    tServoData data;
    data.ui8Angle = (uint8_t)angle;

    std::vector<uint8_t> stuffed_frame = _pack_and_stuff_data(header, data);

    boost::lock_guard<boost::mutex> lock(_serial_mutex);
    size_t written = _port.write(stuffed_frame.data(), int(stuffed_frame.size()));
    if (written == stuffed_frame.size())
    {
        _logger.log_out_frame(0, stuffed_frame);
    }
    else
    {
        _logger.log_out_frame(ERROR_FRAME_NOT_WRITTEN, stuffed_frame);
    }
}

void arduino_com_client::send_speed(float speed)
{
    // remap speed
    speed = speed > 100.f ? 100.f : speed;
    speed = speed < -100.f ? -100.f : speed;
    speed += 100.f;
    speed *= 0.9f;

    tArduinoHeader header;
    header.ui8ID = ID_ARD_ACT_SPEED_CONTR;
    header.ui8DataLength = sizeof(tServoData);
    header.ui32Timestamp = 0;

    tServoData data;
    data.ui8Angle = (uint8_t)speed;

    std::vector<uint8_t> stuffed_frame = _pack_and_stuff_data(header, data);

    boost::lock_guard<boost::mutex> lock(_serial_mutex);
    size_t written = _port.write(stuffed_frame.data(), int(stuffed_frame.size()));
    if (written == stuffed_frame.size())
    {
        _logger.log_out_frame(0, stuffed_frame);
    }
    else
    {
        _logger.log_out_frame(ERROR_FRAME_NOT_WRITTEN, stuffed_frame);
    }
}
 */
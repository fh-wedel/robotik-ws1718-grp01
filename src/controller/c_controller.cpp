
#include "stdafx.h"
#include "c_controller.h"
#include <iostream>
#include "../../protocol.h"
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <aadc_structs.h>


ADTF_FILTER_PLUGIN("Controller", OID_ADTF_CARCONTROLLER, c_controller);


tFloat32 sum_x = 0;
tFloat32 sum_y = 0;
tFloat32 sum_z = 0;
tInt32 samples = 0;

float cur_speed = 0;
float cur_angle = 0;
bool emergeny_break_enabled = 0;

c_controller::c_controller(const tChar *__info) : cFilter(__info) {

}

c_controller::~c_controller() {

}


tResult c_controller::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        /*
        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));
        tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
        RETURN_IF_POINTER_NULL(strDescSignalValue);
        cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionAccelerateSignalInput))
        */
        cObjectPtr<IMediaType> pUSSType;
        RETURN_IF_FAILED(AllocMediaType(&pUSSType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_USS.Create("USS", pUSSType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_USS));

        cObjectPtr<IMediaType> pdiffType;
        RETURN_IF_FAILED(AllocMediaType(&pdiffType, MEDIA_TYPE_LINEDETECTION, MEDIA_SUBTYPE_LINEDETECTIONDIFF, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_diff.Create("diff", pdiffType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_diff));

        /*
		cObjectPtr<IMediaType> pSpeedType;
        RETURN_IF_FAILED(AllocMediaType(&pSpeedType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_speed.Create("speed", pSpeedType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_speed));
        */

        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_MOTORCONTROL, MEDIA_SUBTYPE_MOTORCONTROL, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin_carcontrol.Create("carcontrol", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_carcontrol));
    }
    RETURN_NOERROR;
}

tResult c_controller::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

float c_controller::getSmallerSpeed(float f1, float f2) {
    //WEGEN NEGATIV
    if (f2 > f1) {
        return f2;
    }
    return f1;
}

float setArduinoSpeed(float relativeSpeed) {
    return relativeSpeed * 12 / 100 * -1;
}

float getGaussianWeightedDistance(float sensor_angle, float steering_angle){
    return 2-exp(-3 * pow((sensor_angle - steering_angle),2) );
}

tResult c_controller::OnPinEvent(IPin *pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample *pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oInputPin_USS) {

            UltrasonicStruct uss = receiveData<UltrasonicStruct>(pMediaSample);

            vector<float> backfrontback_ucs(10);
            //vector<float> left_ucs;


            // normierte Geschwindigkeiten [-100 <= x <= 100]


            float ultrasonicNormalisation = 1.0f;

            backfrontback_ucs[0] = uss.tRearCenter.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[1] = uss.tRearLeft.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[2] = uss.tSideLeft.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[3] = uss.tFrontLeft.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[4] = uss.tFrontCenterLeft.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[5] = uss.tFrontCenter.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[6] = uss.tFrontCenterRight.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[7] = uss.tFrontRight.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[8] = uss.tSideRight.f32Value * ultrasonicNormalisation;
            backfrontback_ucs[9] = uss.tRearRight.f32Value * ultrasonicNormalisation;



            /*
            float motorcontr_angle = _motorControl.angle;
            int motorcontr_weight = 1;
            float weighted_percentage = 100;

            for(int i = 0; i < 10; ++i) {
                 if (i >= 5){
                    motorcontr_weight = -1;
                }

                weighted_percentage = motorcontr_angle / 100 *

                backfrontback_ucs[i] = backfrontback_ucs[i]


            }
             */

            // function to weigh the distances:
            // Gauss-Kurve zur Gewichtung der Messwerte

            // konstante Winkel pro Sensor:
            float REAR_CENTER_ANGLE = 1;
            float REAR_LEFT_ANGLE = 1;
            float SIDE_LEFT_ANGLE = 0.7;
            float FRONT_LEFT_ANGLE = 0.4;
            float FRONT_CENTER_LEFT_ANGLE = 0.2;
            float FRONT_CENTER_ANGLE = 0;
            float FRONT_CENTER_RIGHT_ANGLE = 0.2;
            float FRONT_RIGHT_ANGLE = 0.4;
            float SIDE_RIGHT_ANGLE = 0.7;
            float REAR_RIGHT_ANGLE = 1;

            //...
            float steering_angle = 0;


            backfrontback_ucs[0] *= getGaussianWeightedDistance(REAR_CENTER_ANGLE, steering_angle);
            backfrontback_ucs[1] *= getGaussianWeightedDistance(REAR_LEFT_ANGLE, steering_angle); // neuere normierter Abstand
            backfrontback_ucs[2] *= getGaussianWeightedDistance(SIDE_LEFT_ANGLE, steering_angle); // neuere normierter Abstand
            backfrontback_ucs[3] *= getGaussianWeightedDistance(FRONT_LEFT_ANGLE, steering_angle);
            backfrontback_ucs[4] *= getGaussianWeightedDistance(FRONT_CENTER_LEFT_ANGLE, steering_angle);
            backfrontback_ucs[5] *= getGaussianWeightedDistance(FRONT_CENTER_ANGLE, steering_angle);
            backfrontback_ucs[6] *= getGaussianWeightedDistance(FRONT_CENTER_RIGHT_ANGLE, steering_angle);
            backfrontback_ucs[7] *= getGaussianWeightedDistance(FRONT_RIGHT_ANGLE, steering_angle);
            backfrontback_ucs[8] *= getGaussianWeightedDistance(SIDE_RIGHT_ANGLE, steering_angle);
            backfrontback_ucs[9] *= getGaussianWeightedDistance(REAR_RIGHT_ANGLE, steering_angle);



            float minDistanceNorm = 1;
            /*
            for (unsigned int i = 0; i < backfrontback_ucs.size(); ++i) {
                minDistanceNorm = backfrontback_ucs[i] < minDistanceNorm ? backfrontback_ucs[i] : minDistanceNorm;
            }
             */

            cout << "fcgauss= " << getGaussianWeightedDistance(FRONT_CENTER_ANGLE, steering_angle);
            cout << "flgauss= " << getGaussianWeightedDistance(FRONT_LEFT_ANGLE, steering_angle);
            cout << "frontCenter= " << backfrontback_ucs[5] << " frontLeft= " << backfrontback_ucs[3] << endl << endl;


            ///TODO Bis hierhin alles Richtig



            //minDistanceNorm = backfrontback_ucs[5] < backfrontback_ucs[3] ? backfrontback_ucs[5] : backfrontback_ucs[3];

            //cout << "minDistanceNorm= " << minDistanceNorm << endl;


            float linear_speed = 100 * minDistanceNorm;

            //cout << "linear_speed= " << linear_speed << " | ";

            _motorControl.speed = setArduinoSpeed(linear_speed);

            /*
            for(int i = 0; i < 10; ++i) {
                backfrontback_ucs[i] = backfrontback_ucs[i] * (2-exp())

            }
             */


/*

            float resultSpeed = MAX_SPEED;

            switch ((int)uss.tFrontCenter.f32Value){
                case 80 ... 400:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED);
                    break;
                case 50 ... 79:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.75f);
                    break;
                case 25 ... 49:
                    resultSpeed = getSmallerSpeed(resultSpeed, (MAX_SPEED * 0.55f));
                    break;
                default:
                    resultSpeed = getSmallerSpeed(resultSpeed, 0);
                    break;
            }

            switch ((int)uss.tFrontCenterRight.f32Value) {
                case 50 ... 400:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED);
                    break;
                case 25 ... 49:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.75f);
                    break;
                case 10 ... 24:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.55f);
                    break;
                default:
                    resultSpeed = getSmallerSpeed(resultSpeed, 0);
                    break;
            }

            switch ((int)uss.tFrontCenterLeft.f32Value) {
                case 50 ... 400:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED);
                    break;
                case 25 ... 49:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.75f);
                    break;
                case 10 ... 24:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.55f);
                    break;
                default:
                    resultSpeed = getSmallerSpeed(resultSpeed, 0);
                    break;
            }

            switch ((int)uss.tFrontLeft.f32Value){
                case 21 ... 400:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED);
                    break;
                case 8 ... 20:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.55f);
                    break;
                default:
                    resultSpeed = getSmallerSpeed(resultSpeed, 0);
                    break;
            }

            switch ((int)uss.tFrontRight.f32Value){
                case 21 ... 400:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED);
                    break;
                case 8 ... 20:
                    resultSpeed = getSmallerSpeed(resultSpeed, MAX_SPEED * 0.55f);
                    break;
                default:
                    resultSpeed = getSmallerSpeed(resultSpeed, 0);
                    break;
            }

            _motorControl.speed = resultSpeed;
            */

        } else if (pSource == &m_oInputPin_diff) {
            LineDetectionDiff diff = receiveData<LineDetectionDiff>(pMediaSample);
            switch(diff){
                case 0:
                    _motorControl.angle = 0;
                    break;
                case 40 ... 100:
                    _motorControl.angle = 100;
                    break;
                case -100 ... -40:
                    _motorControl.angle = -100;
                    break;
                case 1 ... 39 :
                case -39 ... -1:
                    _motorControl.angle = diff * 100 / 40;
                    break;
                default:
                    break;
            }

        }

        /*
        if (pSource == &m_oInputPin_speed) {
		float f32value;
		static tBufferID szIDF32Value_SpeedContr;
		__adtf_sample_read_lock_mediadescription(m_pDescriptionAccelerateSignalInput, pMediaSample, pCoderInput);
         pCoderInput->GetID("f32Value", szIDF32Value_SpeedContr);
        pCoderInput->Get(szIDF32Value_SpeedContr, (tVoid*)&f32value);
          
        MotorControl test;
		test.speed = f32value;
		sendData<MotorControl>(&m_oOutputPin_carcontrol, &test);
        }
         */


        //Send Data
        //cout << "speed= " << _motorControl.speed << ", angle= " << _motorControl.angle << endl;
        sendData(&m_oOutputPin_carcontrol, &_motorControl);
    }

    RETURN_NOERROR;
}




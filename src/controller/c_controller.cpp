
#include "stdafx.h"
#include "c_controller.h"
#include <iostream>
#include "../../helperFunctions.h"
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <aadc_structs.h>


ADTF_FILTER_PLUGIN("Controller", OID_ADTF_CARCONTROLLER, c_controller);

#define ARRPOS_REAR_CENTER         0
#define ARRPOS_REAR_LEFT           1
#define ARRPOS_SIDE_LEFT           2
#define ARRPOS_FRONT_LEFT          3
#define ARRPOS_FRONT_CENTER_LEFT   4
#define ARRPOS_FRONT_CENTER        5
#define ARRPOS_FRONT_CENTER_RIGHT  6
#define ARRPOS_FRONT_RIGHT         7
#define ARRPOS_SIDE_RIGHT          8
#define ARRPOS_REAR_RIGHT          9

bool emergeny_break_enabled = 0;

c_controller::c_controller(const tChar *__info) : cFilter(__info) {

    // konstante Winkel pro Sensor:
    SetPropertyFloat("REAR_CENTER_ANGLE", sa.REAR_CENTER_ANGLE);
    SetPropertyFloat("REAR_LEFT_ANGLE", sa.REAR_LEFT_ANGLE);
    SetPropertyFloat("SIDE_LEFT_ANGLE", sa.SIDE_LEFT_ANGLE);
    SetPropertyFloat("FRONT_LEFT_ANGLE", sa.FRONT_LEFT_ANGLE);
    SetPropertyFloat("FRONT_CENTER_LEFT_ANGLE", sa.FRONT_CENTER_LEFT_ANGLE);
    SetPropertyFloat("FRONT_CENTER_ANGLE", sa.FRONT_CENTER_ANGLE);
    SetPropertyFloat("FRONT_CENTER_RIGHT_ANGLE", sa.FRONT_CENTER_RIGHT_ANGLE);
    SetPropertyFloat("FRONT_RIGHT_ANGLE", sa.FRONT_CENTER_ANGLE);
    SetPropertyFloat("SIDE_RIGHT_ANGLE", sa.SIDE_RIGHT_ANGLE);
    SetPropertyFloat("REAR_RIGHT_ANGLE", sa.REAR_RIGHT_ANGLE);
}

c_controller::~c_controller() {

}


tResult c_controller::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    sa.REAR_CENTER_ANGLE = (float)GetPropertyFloat("REAR_CENTER_ANGLE");
    sa.REAR_LEFT_ANGLE = (float)GetPropertyFloat("REAR_LEFT_ANGLE");
    sa.SIDE_LEFT_ANGLE = (float)GetPropertyFloat("SIDE_LEFT_ANGLE");
    sa.FRONT_LEFT_ANGLE = (float)GetPropertyFloat("FRONT_LEFT_ANGLE");
    sa.FRONT_CENTER_LEFT_ANGLE = (float)GetPropertyFloat("FRONT_CENTER_LEFT_ANGLE");
    sa.FRONT_CENTER_ANGLE = (float)GetPropertyFloat("FRONT_CENTER_ANGLE");
    sa.FRONT_CENTER_RIGHT_ANGLE = (float)GetPropertyFloat("FRONT_CENTER_RIGHT_ANGLE");
    sa.FRONT_CENTER_ANGLE = (float)GetPropertyFloat("FRONT_RIGHT_ANGLE");
    sa.SIDE_RIGHT_ANGLE = (float)GetPropertyFloat("SIDE_RIGHT_ANGLE");
    sa.REAR_RIGHT_ANGLE = (float)GetPropertyFloat("REAR_RIGHT_ANGLE");


    if (eStage == StageFirst) {

        cObjectPtr<IMediaType> pUSSType;
        RETURN_IF_FAILED(AllocMediaType(&pUSSType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_USS.Create("USS", pUSSType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_USS));

        cObjectPtr<IMediaType> pdiffType;
        RETURN_IF_FAILED(AllocMediaType(&pdiffType, MEDIA_TYPE_LINEDETECTION, MEDIA_SUBTYPE_LINEDETECTIONDIFF, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_diff.Create("diff", pdiffType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_diff));

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

// sensor_angle: [0 - 1]
// steering_angle: [0 - 1]
// Berechnet die durch eine Gauss-Kurve gewichtete Sensor-Wertigkeit
float getGaussianSensorWeight(float sensor_angle, float steering_angle) {
    return 5-4*exp(-3 * pow((sensor_angle - steering_angle),2) );
}

// sensor_angle: [0 - 1]
// steering_angle: [0 - 1]
// Berechnet die durch eine Gauss-Kurve gewichtete Sensor-Wertigkeit
float getGaussianSensorWeight(float sensor_angle, float steering_angle, bool front) {
    return 5-4*exp(-3 * pow((sensor_angle - steering_angle),2) );
}

// dist: zwischen [0 - 400]
// minDistCutoff: [0 - 100] cotoff für die mindestGeschwindigkeit
// return: Geschwindigkeit zwischen [0 - 100]
// Berechnet die Geschwindigkeit aus dem gegebenen Distanzwert und dem Distanz-Schwellwert
float minDistToSpeed(float dist, float minDistCutoff) {
    float speed = 0;

    if (dist < minDistCutoff){ // Distanz unter Cutoff: Geschwindigkeit 0
        speed = 0;
    } else if (dist > 100) { // Distanz größer als 1 Meter: Maximalgeschwindigkeit
        speed = 100;
    } else {
        speed = dist;
        //dist = dist - minDistCutoff;
        //speed = 50/(100-minDistCutoff) * dist + 50.0f;
    }
    return speed;
}

tResult c_controller::OnPinEvent(IPin *pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample *pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oInputPin_USS) {

            UltrasonicStruct uss = receiveData<UltrasonicStruct>(pMediaSample);

            vector<float> us_values(10);

            //
            float steering_angle = _motorControl.angle / 100 * (sa.FRONT_RIGHT_ANGLE - sa.FRONT_CENTER_RIGHT_ANGLE);

            // gewichte Sensor-Input nach Gauss-Kurve
            us_values[ARRPOS_REAR_CENTER] = uss.tRearCenter.f32Value *
                    getGaussianSensorWeight(sa.REAR_CENTER_ANGLE, steering_angle);
            us_values[ARRPOS_REAR_LEFT] = uss.tRearLeft.f32Value *
                    getGaussianSensorWeight(sa.REAR_LEFT_ANGLE, steering_angle);
            us_values[ARRPOS_SIDE_LEFT] = uss.tSideLeft.f32Value *
                    getGaussianSensorWeight(sa.SIDE_LEFT_ANGLE, steering_angle);
            us_values[ARRPOS_FRONT_LEFT] = uss.tFrontLeft.f32Value *
                    getGaussianSensorWeight(sa.FRONT_LEFT_ANGLE, steering_angle);
            us_values[ARRPOS_FRONT_CENTER_LEFT] = uss.tFrontCenterLeft.f32Value *
                    getGaussianSensorWeight(sa.FRONT_CENTER_LEFT_ANGLE, steering_angle);;
            us_values[ARRPOS_FRONT_CENTER] = uss.tFrontCenter.f32Value *
                    getGaussianSensorWeight(sa.FRONT_CENTER_ANGLE, steering_angle);;
            us_values[ARRPOS_FRONT_CENTER_RIGHT] = uss.tFrontCenterRight.f32Value *
                    getGaussianSensorWeight(sa.FRONT_CENTER_RIGHT_ANGLE, steering_angle);
            us_values[ARRPOS_FRONT_RIGHT] = uss.tFrontRight.f32Value *
                    getGaussianSensorWeight(sa.FRONT_RIGHT_ANGLE, steering_angle);
            us_values[ARRPOS_SIDE_RIGHT] = uss.tSideRight.f32Value *
                    getGaussianSensorWeight(sa.SIDE_RIGHT_ANGLE, steering_angle);
            us_values[ARRPOS_REAR_RIGHT] = uss.tRearRight.f32Value *
                    getGaussianSensorWeight(sa.REAR_RIGHT_ANGLE, steering_angle);


            float minDistance = 800;
            for (unsigned int i = 0; i < us_values.size(); ++i) {
                minDistance = us_values[i] < minDistance ? us_values[i] : minDistance;
            }
            // auf 0 - 100 normierter mindest-Distanzwert
            float minDistanceNorm = minDistance; // / 4

            cout << "minDistanceNorm= " << minDistanceNorm << " | ";

            // Berechnet die Geschwindigkeit anhand der minimalsten Distanz und des mindest-Schwellwertes
			float speed = minDistToSpeed(minDistanceNorm, 15);

            cout << "speed= " << speed << " | ";

            _motorControl.speed = speed;

            //cout << "motorspeed= " << _motorControl.speed << endl;


        } else if (pSource == &m_oInputPin_diff) {
            LineDetectionDiff difference = receiveData<LineDetectionDiff>(pMediaSample);


            switch(difference){
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
                    _motorControl.angle = difference * 100 / 40;
                    break;
                default:
                    break;
            }

        }


        //Send Data
        //cout << "speed= " << _motorControl.speed << ", angle= " << _motorControl.angle << endl;
        sendData(&m_oOutputPin_carcontrol, &_motorControl);
    }

    RETURN_NOERROR;
}





#include "stdafx.h"
#include "c_accelerationFilter.h"
#include "../../protocol.h"
#include <stdio.h>
#include <cmath>



ADTF_FILTER_PLUGIN("AccelerationFilter", OID_ADTF_ACCELERATION_FILTER, c_accelerationFilter);


tFloat32 sum_x = 0;
tFloat32 sum_y = 0;
tFloat32 sum_z = 0;
tInt32 samples = 0;


c_accelerationFilter::c_accelerationFilter(const tChar *__info) : cFilter(__info) {

}

c_accelerationFilter::~c_accelerationFilter() {

}


tResult c_accelerationFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {
        cObjectPtr<IMediaType> pAccelerationlType;
        RETURN_IF_FAILED(AllocMediaType(&pAccelerationlType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin_acceleration.Create("acc_in", pAccelerationlType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_acceleration));

        cObjectPtr<IMediaType> pFlagType;
        RETURN_IF_FAILED(AllocMediaType(&pFlagType, MEDIA_TYPE_FLAG, MEDIA_SUBTYPE_FLAG, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin_flags.Create("flag_out", pFlagType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_flags));
    }
    RETURN_NOERROR;
}

tResult c_accelerationFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}


tResult c_accelerationFilter::OnPinEvent(IPin *pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample *pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {

        if (pSource == &m_oInputPin_acceleration) {

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

            if (acc_z > ACC_Z_NORMAL + 5) {
				printf("%f\n", acc_z);
                Flag flag = FLAG_EMERGENCY_BREAK;
                sendData<Flag>(&m_oOutputPin_flags, &flag);
            }
        }
    }
    RETURN_NOERROR;
}

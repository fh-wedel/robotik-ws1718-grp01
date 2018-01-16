

#include "stdafx.h"
#include "ultrasonicLogic.h"
#include <aadc_structs.h>
#include "../../helperFunctions.h"

#include <iostream>


ADTF_FILTER_PLUGIN("UltrasonicLogic", OID_ADTF_TEMPLATE_FILTER, cUltrasonicLogic);


cUltrasonicLogic::cUltrasonicLogic(const tChar *__info) {
    SetPropertyInt("MedianFilterListLength", 5);
    SetPropertyInt("MedianFilterListInit", 400);

    SetPropertyInt("STOP_distance_Front", 100);
    SetPropertyInt("STOP_distance_Side", 50);
    SetPropertyInt("STOP_distance_Rear", 100);
}

cUltrasonicLogic::~cUltrasonicLogic() {

}

tResult cUltrasonicLogic::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    frontCenterFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    frontCenterLeftFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    frontLeftFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    sideLeftFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    rearLeftFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    rearCenterFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    rearRightFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    sideRightFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    frontRightFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));
    frontCenterRightFilter.initMedianFilter((uint64_t) GetPropertyInt("MedianFilterListLength"), GetPropertyInt("MedianFilterListInit"));

    if (eStage == StageFirst) {
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin.Create("ultrasonicStruct_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        cObjectPtr<IMediaType> pOutputTypeUltrasonicStruct;
        RETURN_IF_FAILED(AllocMediaType(&pOutputTypeUltrasonicStruct, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin.Create("ultrasonicStruct_out", pOutputTypeUltrasonicStruct, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));

        cObjectPtr<IMediaType> pOutputFlag;
        RETURN_IF_FAILED(AllocMediaType(&pOutputTypeUltrasonicStruct, MEDIA_TYPE_FLAG, MEDIA_SUBTYPE_FLAG, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputEmergencyFlagPin.Create("flag_out", pOutputTypeUltrasonicStruct, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputEmergencyFlagPin));
    }
    RETURN_NOERROR;
}

tResult cUltrasonicLogic::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cUltrasonicLogic::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        if (pSource == &m_oInputPin) {

            UltrasonicStruct receivedUltrasonicStruct = receiveData<UltrasonicStruct>(pMediaSample);
			receivedUltrasonicStruct.tFrontCenterLeft.f32Value = 400;
            if (receivedUltrasonicStruct.tFrontCenter.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tFrontCenter.f32Value = frontCenterFilter.putNewValue(fabs(receivedUltrasonicStruct.tFrontCenter.f32Value));
            }

            if (receivedUltrasonicStruct.tFrontCenterLeft.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tFrontCenterLeft.f32Value = frontCenterLeftFilter.putNewValue(fabs(receivedUltrasonicStruct.tFrontCenterLeft.f32Value));
            }

            if (receivedUltrasonicStruct.tFrontLeft.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tFrontLeft.f32Value = frontLeftFilter.putNewValue(fabs(receivedUltrasonicStruct.tFrontLeft.f32Value));
            }

            if (receivedUltrasonicStruct.tSideLeft.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tSideLeft.f32Value = sideLeftFilter.putNewValue(fabs(receivedUltrasonicStruct.tSideLeft.f32Value));
            }

            if (receivedUltrasonicStruct.tRearLeft.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tRearLeft.f32Value = rearLeftFilter.putNewValue(fabs(receivedUltrasonicStruct.tRearLeft.f32Value));
            }

            if (receivedUltrasonicStruct.tRearCenter.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tRearCenter.f32Value = rearCenterFilter.putNewValue(fabs(receivedUltrasonicStruct.tRearCenter.f32Value));
            }

            if (receivedUltrasonicStruct.tRearRight.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tRearRight.f32Value = rearRightFilter.putNewValue(fabs(receivedUltrasonicStruct.tRearRight.f32Value));
            }

            if (receivedUltrasonicStruct.tSideRight.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tSideRight.f32Value = sideRightFilter.putNewValue(fabs(receivedUltrasonicStruct.tSideRight.f32Value));
            }

            if (receivedUltrasonicStruct.tFrontRight.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tFrontRight.f32Value = frontRightFilter.putNewValue(fabs(receivedUltrasonicStruct.tFrontRight.f32Value));
            }

            if (receivedUltrasonicStruct.tFrontCenterRight.f32Value >= (0 - SIG_ATOMIC_MAX)) {
                _ultrasonicStruct.tFrontCenterRight.f32Value = frontCenterRightFilter.putNewValue(fabs(receivedUltrasonicStruct.tFrontCenterRight.f32Value));
            }

            if (       (_ultrasonicStruct.tFrontCenter.f32Value < GetPropertyInt("STOP_distance_Front"))
                    || (_ultrasonicStruct.tFrontCenterLeft.f32Value < GetPropertyInt("STOP_distance_Front"))
                    || (_ultrasonicStruct.tFrontLeft.f32Value < GetPropertyInt("STOP_distance_Front"))
                    || (_ultrasonicStruct.tSideLeft.f32Value < GetPropertyInt("STOP_distance_Side"))
                    || (_ultrasonicStruct.tRearLeft.f32Value < GetPropertyInt("STOP_distance_Rear"))
                    || (_ultrasonicStruct.tRearCenter.f32Value < GetPropertyInt("STOP_distance_Rear"))
                    || (_ultrasonicStruct.tRearRight.f32Value < GetPropertyInt("STOP_distance_Rear"))
                    || (_ultrasonicStruct.tSideRight.f32Value < GetPropertyInt("STOP_distance_Side"))
                    || (_ultrasonicStruct.tFrontRight.f32Value < GetPropertyInt("STOP_distance_Front"))
                    || (_ultrasonicStruct.tFrontCenterRight.f32Value < GetPropertyInt("STOP_distance_Front"))) {
                Flag flag = FLAG_EMERGENCY_BREAK_USS;
                cout << "UltrasonicLogik:: FLAG_EMERGENCY_BREAK" << endl;
                sendData<Flag>(&m_oOutputEmergencyFlagPin, &flag);
            } else {
			Flag flag = FLAG_EMERGENCY_BREAK_USS_OK;
                sendData<Flag>(&m_oOutputEmergencyFlagPin, &flag);
			}
            sendData<UltrasonicStruct>(&m_oOutputPin, &_ultrasonicStruct);
        }
    }
    RETURN_NOERROR;
}

//
// Created by aadc on 20.11.17.
//

#ifndef PROJECT_PROTOCOL_H
#define PROJECT_PROTOCOL_H

#include <stdint.h>
#include <adtf_plugin_sdk.h>
#include <aadc_structs.h>
#include <sdk/mediaallochelper.h>
#include <sdk/filter.h>

#define MEDIA_TYPE_ULTRASONICSTRUCT     MEDIA_HANDLE_UNKNOWN
#define MEDIA_SUBTYPE_ULTRASONICSTRUCT  MEDIA_HANDLE_UNKNOWN

#define MEDIA_TYPE_FLAG                 MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_FLAG              0

#define MEDIA_TYPE_MOTORCONTROL         MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_MOTORCONTROL      1

#define MEDIA_TYPE_LINEDETECTION        MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_LINEDETECTIONDIFF 2


typedef struct {
    float angle;
    float speed;
} MotorControl;

typedef uint64_t Flag;
typedef int64_t LineDetectionDiff;
typedef tUltrasonicStruct UltrasonicStruct;

template <typename T> bool sendData(adtf::cPin &outPin, T* data) {
    cObjectPtr<adtf::IMediaSample> pNewSample;
    if (IS_OK(adtf::cMediaAllocHelper::AllocMediaSample(&pNewSample)))
    {
        pNewSample->Update(0, data, sizeof(T), 0);
        outPin.Transmit(pNewSample);
        return true;
    }
    return false;
}

template <typename T> T receiveData(adtf::IMediaSample* pSample) {
    T* rData = NULL;
    T rDataCopy;
    if (IS_OK(pSample->Lock((const tVoid**)&rData))) {
        memcpy(&rDataCopy, rData, sizeof(T));
        pSample->Unlock(rData);
    }
    return rDataCopy;
}


#endif //PROJECT_PROTOCOL_H

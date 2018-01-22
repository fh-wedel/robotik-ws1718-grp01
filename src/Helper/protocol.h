//
// Created by aadc on 20.11.17.
//

#ifndef PROJECT_PROTOCOL_H
#define PROJECT_PROTOCOL_H

#include "stdafx.h"
#include <stdint.h>
#include <adtf_plugin_sdk.h>
#include <aadc_structs.h>
#include <sdk/mediaallochelper.h>
#include <sdk/filter.h>
#include "ADTF_OpenCV_helper.h"
#include <adtf_platform_inc.h>


#define MEDIA_TYPE_ULTRASONICSTRUCT     MEDIA_HANDLE_UNKNOWN
#define MEDIA_SUBTYPE_ULTRASONICSTRUCT  MEDIA_HANDLE_UNKNOWN

#define MEDIA_TYPE_FLAG                 MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_FLAG              0

#define MEDIA_TYPE_MOTORCONTROL         MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_MOTORCONTROL      1

#define MEDIA_TYPE_LINEDETECTION        MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_LINEDETECTIONDIFF 2

#define MEDIA_TYPE_FILTERVALUE          MEDIA_TYPE_USER
#define MEDIA_SUBTYPE_FILTERVALUE       3

#define MEDIA_TYPE_OTHER 0x00000000

#define FLAG_EMERGENCY_BREAK_USS 1

#define FLAG_EMERGENCY_BREAK_ACC 2

#define FLAG_EMERGENCY_BREAK_USS_OK 1




typedef struct {
    //Winkel von -100 bis 100
    float angle;
    //Speed von -100 bis 100
    float speed;
} MotorControl;

typedef tFloat32 FilterValue;

typedef uint64_t Flag;

//Wert zwischen -100 und 100
typedef int64_t LineDetectionDiff;

typedef tUltrasonicStruct UltrasonicStruct;

/**
 * Senden einer Nachricht vom Typ T ueber einen bestimmten Pin.
 * @tparam T Typ der Nachricht
 * @param outPin Output Pin
 * @param data Daten
 * @return erfolgreiches Senden
 */
template <typename T> bool sendData(adtf::cPin *outPin, T* data) {
    cObjectPtr<adtf::IMediaSample> pNewSample;
    if (IS_OK(adtf::cMediaAllocHelper::AllocMediaSample(&pNewSample)))
    {
        //Update Kopiert die Daten (laut Doku)
        pNewSample->Update(0, data, sizeof(T), 0);
        outPin->Transmit(pNewSample);
        return true;
    }
    return false;
}

/**
 * Senden eines Bildes ueber einen bestimmten Pin.
 * @param outPin Output Pin
 * @param data Mat Bild
 * @return erfolgreiches Senden
 */
bool sendData(adtf::cVideoPin *outPin, cv::Mat *data) {
    cObjectPtr<adtf::IMediaSample> pNewSample;
    static tBitmapFormat sOutputFormat;
    if (IS_OK(adtf::cMediaAllocHelper::AllocMediaSample(&pNewSample))) {
        Mat2BmpFormat(*data, sOutputFormat);
        outPin->SetFormat(&sOutputFormat, NULL);
        cImage newImage;
        newImage.Create(sOutputFormat.nWidth, sOutputFormat.nHeight, sOutputFormat.nBitsPerPixel, sOutputFormat.nBytesPerLine, data->data);

        //Update Kopiert die Daten (laut Doku)
        pNewSample->Update(0, newImage.GetBitmap(), newImage.GetSize(), 0);
        outPin->Transmit(pNewSample);
        //Bei Speicherleck doch wichtig!
        //data->release();
        return true;
    }
    return false;
}

/**
 * Empfangen einer Nachricht vom Typ T.
 * @tparam T Typ der Nachricht
 * @param pSample Sample
 * @return Kopie der Daten
 */
template <typename T> T receiveData(adtf::IMediaSample* pSample) {
    T* rData = NULL;
    T rDataCopy = T();
    if (IS_OK(pSample->Lock((const tVoid**)&rData))) {
        memcpy(&rDataCopy, rData, sizeof(T));
        pSample->Unlock(rData);
    }
    return rDataCopy;
}

/**
 * Empfangen eines Bildes an einem bestimmten Pin.
 * @param inputPin Input Pin
 * @param pSample Sample
 * @return Kopie des Bildes
 */
cv::Mat receiveData(adtf::cVideoPin *inputPin, adtf::IMediaSample* pSample) {

    cv::Mat newMat;
    const tBitmapFormat inputFormat = *inputPin->GetFormat();
    //create the input matrix
    BmpFormat2Mat(inputFormat, newMat);

    const void* rData;

    if (IS_OK(pSample->Lock((const tVoid**)&rData))) {
        memcpy(newMat.data, rData, size_t(inputFormat.nSize));
        pSample->Unlock(rData);
    }
    return newMat;
}

cCriticalSection m_critSecTransmitControl;
cObjectPtr<IMediaTypeDescription> m_pDescriptionFloat;


tResult ArduinoTransmitFloatValue(cOutputPin* oPin, tFloat32 value, tUInt32 timestamp)
{
    //use mutex
    __synchronized_obj(m_critSecTransmitControl);

    cObjectPtr<IMediaSample> pMediaSample;
    adtf::cMediaAllocHelper::AllocMediaSample((tVoid**)&pMediaSample);

    cObjectPtr<IMediaSerializer> pSerializer;
    
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    //WARNING NULL is a fix
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,NULL));
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionFloat));
    
    
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

    pMediaSample->SetTime(0);

    oPin->Transmit(pMediaSample);

    RETURN_NOERROR;
}




#endif //PROJECT_PROTOCOL_H

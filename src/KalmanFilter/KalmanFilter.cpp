
#include <math.h>
#include "stdafx.h"
#include "KalmanFilter.h"
#include <aadc_structs.h>
#include "../../protocol.h"

#include <iostream>


ADTF_FILTER_PLUGIN("KalmanFilter", OID_ADTF_TEMPLATE_FILTER, cKalmanFilter);


cKalmanFilter::cKalmanFilter(const tChar* __info):cFilter(__info) {
    SetPropertyInt("processNoise", 0);
    SetPropertyStr("processNoise" NSSUBPROP_DESCRIPTION, "Prozessrauschen");
    SetPropertyInt("processNoise" NSSUBPROP_MIN, 0);
    SetPropertyInt("processNoise" NSSUBPROP_MAX, 1000);
}

cKalmanFilter::~cKalmanFilter() {

}

tResult cKalmanFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    //_processNoise = (unsigned int)GetPropertyInt("processNoise");
    _sigma_mess = 1;
    _sigma_move = 2;

    _mu = 400;
    _sig = 10000;

    if (eStage == StageFirst) {

        cObjectPtr<IMediaType> pInputTypeUltrasonnicStruct;
        RETURN_IF_FAILED(AllocMediaType(&pInputTypeUltrasonnicStruct, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin.Create("ultrasonicStruct", pInputTypeUltrasonnicStruct, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        cObjectPtr<IMediaType> pInputTypeRequest;
        RETURN_IF_FAILED(AllocMediaType(&pInputTypeRequest, MEDIA_TYPE_REQUEST, MEDIA_SUBTYPE_REQUEST, __exception_ptr));
        RETURN_IF_FAILED(m_oRequestPin.Create("request", pInputTypeRequest, this));
        RETURN_IF_FAILED(RegisterPin(&m_oRequestPin));


        cObjectPtr<IMediaType> pOutputTypeUltrasonnicStruct;
        RETURN_IF_FAILED(AllocMediaType(&pOutputTypeUltrasonnicStruct, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin.Create("filteredUltrasonicStruct", pOutputTypeUltrasonnicStruct, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));
    }

    RETURN_NOERROR;
}

tResult cKalmanFilter::Shutdown(tInitStage eStage, __exception) {
    if (eStage == StageGraphReady) {

    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}

void cKalmanFilter::update(tFloat32 newVal) {
    _mu = ((_sigma_mess * _mu) + (_sig * newVal)) / (_sig + _sigma_mess);
    _sig = 1.0f / ((1.0f /_sig) + (1.0f / _sigma_mess));
}

void cKalmanFilter::predict(tFloat32 newVal) {
    _mu = _mu + newVal;
    _sig = _sig + _sigma_mess*10;
}


tResult cKalmanFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {

    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputPin) {
            //Updateschritt
            tUltrasonicStruct* pSampleData = NULL;
            if (IS_OK(pMediaSample->Lock((const tVoid**)&pSampleData))) {
                tFloat32 newVal = pSampleData->tFrontLeft.f32Value;
                pMediaSample->Unlock(pSampleData);

                //_mu = ((_sigma_mess * _mu) + (_sig * newVal)) / (_sig + _sigma_mess);
                //_sig = 1.0f / ((1.0f /_sig) + (1.0f / _sigma_mess));

                update(newVal);
                predict(_mu - newVal);

                //tUltrasonicStruct filteredUltrasonicStruct;
                //filteredUltrasonicStruct.tFrontLeft.f32Value = _mu;

                cout << newVal << " -- " << _mu << ", sig= " << _sig << endl;

                //Senden des neuen gefilterten UltrasonicStructs
                /*cObjectPtr<IMediaSample> pNewSample;
                if (IS_OK(AllocMediaSample(&pNewSample)))
                {
                    pNewSample->Update(pMediaSample->GetTime(), &filteredUltrasonicStruct, sizeof(filteredUltrasonicStruct), 0);
                    m_oOutputPin.Transmit(pNewSample);
                }
                 */


            }
        } else if (pSource == &m_oRequestPin) {
            //Schaetzung

        }

    }

    RETURN_NOERROR;
}

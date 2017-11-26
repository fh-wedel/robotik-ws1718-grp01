

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
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    //_processNoise = (unsigned int)GetPropertyInt("processNoise");

    // in StageFirst you can create and register your static pins.
    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_Kalman_1D, MEDIA_SUBTYPE_Kalman_1D, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin.Create("values", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        RETURN_IF_FAILED(m_oRequestPin.Create("request", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oRequestPin));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_Kalman_1D, MEDIA_SUBTYPE_Kalman_1D, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin.Create("filteredValue", pOutputType, this));
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

tResult cKalmanFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {

    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputPin) {
            //Updateschritt
            tUltrasonicStruct* pSampleData = NULL;
            if (IS_OK(pMediaSample->Lock((const tVoid**)&pSampleData))) {
                tFloat32 tmpVal = pSampleData->tFrontLeft.f32Value;
                pMediaSample->Unlock(pSampleData);
                tmpVal++;
                //_mean = ()





            }
        } else if (pSource == &m_oRequestPin) {
            //Schaetzung

        }

    }

    RETURN_NOERROR;
}

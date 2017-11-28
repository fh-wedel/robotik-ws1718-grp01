

#include "stdafx.h"
#include "ultrasonicLogic.h"
#include <aadc_structs.h>
#include "../../protocol.h"

#include <iostream>


ADTF_FILTER_PLUGIN("UltrasonicLogic", OID_ADTF_TEMPLATE_FILTER, cUltrasonicLogic);


cUltrasonicLogic::cUltrasonicLogic(const tChar *__info) {

}

cUltrasonicLogic::~cUltrasonicLogic() {

}

tResult cUltrasonicLogic::Init(tInitStage eStage, __exception) {
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))



    // in StageFirst you can create and register your static pins.
    if (eStage == StageFirst) {

        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oInputPin.Create("ultrasonicStruct", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));
        RETURN_IF_FAILED(m_oRequestPin.Create("request", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oRequestPin));


        cObjectPtr<IMediaType> pOutputTypeUltrasonicStruct;
        RETURN_IF_FAILED(AllocMediaType(&pOutputTypeUltrasonicStruct, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        RETURN_IF_FAILED(m_oOutputPin.Create("outputDUMMY", pOutputTypeUltrasonicStruct, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));

        cObjectPtr<IMediaType> pOutputTypeFlag;
        RETURN_IF_FAILED(AllocMediaType(&pOutputTypeFlag, MEDIA_TYPE_REQUEST, MEDIA_SUBTYPE_REQUEST, __exception_ptr));
        RETURN_IF_FAILED(m_oEmergencyFlagPin.Create("EmergencyFlag", pOutputTypeFlag, this));
        RETURN_IF_FAILED(RegisterPin(&m_oEmergencyFlagPin));
    }

    RETURN_NOERROR;
}

tResult cUltrasonicLogic::Shutdown(tInitStage eStage, __exception) {
    if (eStage == StageGraphReady) {

    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cUltrasonicLogic::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {

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

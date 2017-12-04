

#include "stdafx.h"
#include "ultrasonicLogic.h"
#include <aadc_structs.h>
#include "../../protocol.h"

#include <iostream>


ADTF_FILTER_PLUGIN("UltrasonicLogic", OID_ADTF_TEMPLATE_FILTER, cUltrasonicLogic);


cUltrasonicLogic::cUltrasonicLogic(const tChar *__info) {
    SetPropertyInt("STOP_distance", 100);
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
    if (eStage == StageGraphReady) {

    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cUltrasonicLogic::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {

    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputPin) {

            UltrasonicStruct ultrasonicStruct = receiveData<UltrasonicStruct>(pMediaSample);

            if (       ultrasonicStruct.tFrontCenter.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tFrontCenterLeft.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tFrontLeft.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tSideLeft.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tRearLeft.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tRearCenter.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tRearRight.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tSideRight.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tFrontRight.f32Value < GetPropertyInt("STOP_distance")
                    || ultrasonicStruct.tFrontCenterRight.f32Value < GetPropertyInt("STOP_distance")) {
                Flag flag = FLAG_EMERGENCY_BREAK;
                cout << "UltrasonicLogik:: FLAG_EMERGENCY_BREAK" << endl;
                sendData<Flag>(&m_oOutputEmergencyFlagPin, &flag);
            }
            sendData<UltrasonicStruct>(&m_oOutputPin, &ultrasonicStruct);

        }

    }

    RETURN_NOERROR;
}

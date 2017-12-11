

#include "stdafx.h"
#include "ultrasonicSplit.h"
#include <aadc_structs.h>
#include "../../protocol.h"


ADTF_FILTER_PLUGIN("ultrasonicSplit", OID_ADTF_ULTRASONICSPLIT, cUltrasonicSplit);


cUltrasonicSplit::cUltrasonicSplit(const tChar* __info):cFilter(__info) {

}

cUltrasonicSplit::~cUltrasonicSplit() {
}

tResult cUltrasonicSplit::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))


    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin.Create("UltrasonicStruct", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_FILTERVALUE, MEDIA_SUBTYPE_FILTERVALUE, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPinFrontCenter.Create("FrontCenter", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinFrontCenter));

        RETURN_IF_FAILED(m_oOutputPinFrontCenterLeft.Create("FrontCenterLeft", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinFrontCenterLeft));

        RETURN_IF_FAILED(m_oOutputPinFrontLeft.Create("FrontLeft", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinFrontLeft));

        RETURN_IF_FAILED(m_oOutputPinSideLeft.Create("SideLeft", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinSideLeft));

        RETURN_IF_FAILED(m_oOutputPinRearLeft.Create("RearLeft", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinRearLeft));

        RETURN_IF_FAILED(m_oOutputPinRearCenter.Create("RearCenter", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinRearCenter));

        RETURN_IF_FAILED(m_oOutputPinSideRight.Create("SideRight", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinSideRight));

        RETURN_IF_FAILED(m_oOutputPinRearRight.Create("RearRight", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinRearRight));

        RETURN_IF_FAILED(m_oOutputPinFrontRight.Create("FrontRight", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinFrontRight));

        RETURN_IF_FAILED(m_oOutputPinFrontCenterRight.Create("FrontCenterRight", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPinFrontCenterRight));
    }

    RETURN_NOERROR;
}

tResult cUltrasonicSplit::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cUltrasonicSplit::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        if (pSource == &m_oInputPin) {
            UltrasonicStruct receivedUltrasonicStruct = receiveData<UltrasonicStruct>(pMediaSample);
            sendData<FilterValue>(&m_oOutputPinFrontCenter, &receivedUltrasonicStruct.tFrontCenter.f32Value);
            sendData<FilterValue>(&m_oOutputPinFrontCenterLeft, &receivedUltrasonicStruct.tFrontCenterLeft.f32Value);
            sendData<FilterValue>(&m_oOutputPinFrontLeft, &receivedUltrasonicStruct.tFrontLeft.f32Value);
            sendData<FilterValue>(&m_oOutputPinSideLeft, &receivedUltrasonicStruct.tSideLeft.f32Value);
            sendData<FilterValue>(&m_oOutputPinRearLeft, &receivedUltrasonicStruct.tRearLeft.f32Value);
            sendData<FilterValue>(&m_oOutputPinRearCenter, &receivedUltrasonicStruct.tRearCenter.f32Value);
            sendData<FilterValue>(&m_oOutputPinRearRight, &receivedUltrasonicStruct.tRearRight.f32Value);
            sendData<FilterValue>(&m_oOutputPinSideRight, &receivedUltrasonicStruct.tSideRight.f32Value);
            sendData<FilterValue>(&m_oOutputPinFrontRight, &receivedUltrasonicStruct.tFrontRight.f32Value);
            sendData<FilterValue>(&m_oOutputPinFrontCenterRight, &receivedUltrasonicStruct.tFrontCenterRight.f32Value);
        }
    }
    RETURN_NOERROR;
}

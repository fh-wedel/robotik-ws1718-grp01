

#include "stdafx.h"
#include "../../helperFunctions.h
#include "ultrasonicMerge.h"
#include <aadc_structs.h>



ADTF_FILTER_PLUGIN("ultrasonicMerge", OID_ADTF_ULTRASONICMERGE, cUltrasonicMerge);


cUltrasonicMerge::cUltrasonicMerge(const tChar* __info):cFilter(__info) {

}

cUltrasonicMerge::~cUltrasonicMerge() {
}

tResult cUltrasonicMerge::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    _bitTest = 0;

    if (eStage == StageFirst) {

        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_FILTERVALUE, MEDIA_SUBTYPE_FILTERVALUE, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oInputPinFrontCenter.Create("FrontCenter", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinFrontCenter));

        RETURN_IF_FAILED(m_oInputPinFrontCenterLeft.Create("FrontCenterLeft", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinFrontCenterLeft));

        RETURN_IF_FAILED(m_oInputPinFrontLeft.Create("FrontLeft", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinFrontLeft));

        RETURN_IF_FAILED(m_oInputPinSideLeft.Create("SideLeft", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinSideLeft));

        RETURN_IF_FAILED(m_oInputPinRearLeft.Create("RearLeft", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinRearLeft));

        RETURN_IF_FAILED(m_oInputPinRearCenter.Create("RearCenter", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinRearCenter));

        RETURN_IF_FAILED(m_oInputPinSideRight.Create("SideRight", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinSideRight));

        RETURN_IF_FAILED(m_oInputPinRearRight.Create("RearRight", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinRearRight));

        RETURN_IF_FAILED(m_oInputPinFrontRight.Create("FrontRight", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinFrontRight));

        RETURN_IF_FAILED(m_oInputPinFrontCenterRight.Create("FrontCenterRight", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPinFrontCenterRight));

        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));
        // create and register the input pin
        RETURN_IF_FAILED(m_oOutputPin.Create("UltrasonicStruct", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));
    }

    RETURN_NOERROR;
}

tResult cUltrasonicMerge::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cUltrasonicMerge::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        uint32_t tmpBit = 0;
        if (pSource == &m_oInputPinFrontCenter) {
            _ultrasonicStruct.tFrontCenter.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 0;
        } else if (pSource == &m_oInputPinFrontCenterLeft) {
            _ultrasonicStruct.tFrontCenterLeft.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 1;
        } else if (pSource == &m_oInputPinFrontLeft) {
            _ultrasonicStruct.tFrontLeft.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 2;
        } else if (pSource == &m_oInputPinSideLeft) {
            _ultrasonicStruct.tSideLeft.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 3;
        } else if (pSource == &m_oInputPinRearLeft) {
            _ultrasonicStruct.tRearLeft.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 4;
        } else if (pSource == &m_oInputPinRearCenter) {
            _ultrasonicStruct.tRearCenter.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 5;
        } else if (pSource == &m_oInputPinRearRight) {
            _ultrasonicStruct.tRearRight.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 6;
        } else if (pSource == &m_oInputPinSideRight) {
            _ultrasonicStruct.tSideRight.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 7;
        } else if (pSource == &m_oInputPinFrontRight) {
            _ultrasonicStruct.tFrontRight.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 8;
        } else if (pSource == &m_oInputPinFrontCenterRight) {
            _ultrasonicStruct.tFrontCenterRight.f32Value = receiveData<FilterValue>(pMediaSample);
            tmpBit |= 1 << 9;
        }

        if (((tmpBit & _bitTest) != 0) || ((tmpBit | _bitTest) == 1023)) {
            _bitTest = 0;
            sendData<UltrasonicStruct>(&m_oOutputPin, &_ultrasonicStruct);
        } else {
            _bitTest |= tmpBit;
        }
    }
    RETURN_NOERROR;
}

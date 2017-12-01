

#include "stdafx.h"
#include "medianFilter.h"
#include <aadc_structs.h>
#include "../../protocol.h"


ADTF_FILTER_PLUGIN("medianFilter", OID_ADTF_MEDIAN_FILTER, cMedianFilter);


cMedianFilter::cMedianFilter(const tChar* __info):cFilter(__info) {
    //Property fuer die Median-Listen-Laenge
    SetPropertyInt("list_length", 10);
    SetPropertyStr("list_length" NSSUBPROP_DESCRIPTION, "length of the median filter list");
    SetPropertyInt("list_length" NSSUBPROP_MIN, 1);
    SetPropertyInt("list_length" NSSUBPROP_MAX, 1000);
}

cMedianFilter::~cMedianFilter() {
}

tResult cMedianFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    //erzeugen der Listen mit der Laenge aus dem Property 'list_length'
    unsigned int tmpListLength = (unsigned int)GetPropertyInt("list_length");
    _list_FrontCenter.resize(tmpListLength);
    _list_FrontCenterLeft.resize(tmpListLength);
    _list_FrontLeft.resize(tmpListLength);
    _list_SideLeft.resize(tmpListLength);
    _list_RearLeft.resize(tmpListLength);
    _list_RearCenter.resize(tmpListLength);
    _list_RearRight.resize(tmpListLength);
    _list_SideRight.resize(tmpListLength);
    _list_FrontRight.resize(tmpListLength);
    _list_FrontCenterRight.resize(tmpListLength);

    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin.Create("ultrasonicStruct", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin.Create("filteredUltrasonicStruct", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));
    }

    RETURN_NOERROR;
}

tResult cMedianFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

/**
 * Gibt den Median der uebergebenen Liste zurueck.
 *
 * @param vec Liste
 * @return Median der Liste
 */
tFloat32 sortCopyMedian(vector<tFloat32> vec) {
    vector<tFloat32> tmpList = vec;
    sort(tmpList.begin(), tmpList.end());
    return tmpList[tmpList.size()/2];
}

/**
 * Rotiert die Liste um einen nach rechts und ersetzt den ersten Wert.
 *
 * @param list Referenz auf die Liste
 * @param newVal neuer einzufuegener Wert
 */
void rotateListInsertNewVal(vector<tFloat32> *list, tFloat32 newVal) {
    rotate(list->begin(), list->begin() + list->size() - 1, list->end());
    (*list)[0] = newVal;
}

tResult cMedianFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputPin) {

            UltrasonicStruct receivedUltrasonicStruct = receiveData<UltrasonicStruct>(pMediaSample);
            rotateListInsertNewVal(&_list_FrontCenter, receivedUltrasonicStruct.tFrontCenter.f32Value);
            rotateListInsertNewVal(&_list_FrontCenterLeft, receivedUltrasonicStruct.tFrontCenterLeft.f32Value);
            rotateListInsertNewVal(&_list_FrontLeft, receivedUltrasonicStruct.tFrontLeft.f32Value);
            rotateListInsertNewVal(&_list_SideLeft, receivedUltrasonicStruct.tSideLeft.f32Value);
            rotateListInsertNewVal(&_list_RearLeft, receivedUltrasonicStruct.tRearLeft.f32Value);
            rotateListInsertNewVal(&_list_RearCenter, receivedUltrasonicStruct.tRearCenter.f32Value);
            rotateListInsertNewVal(&_list_RearRight, receivedUltrasonicStruct.tRearRight.f32Value);
            rotateListInsertNewVal(&_list_SideRight, receivedUltrasonicStruct.tSideRight.f32Value);
            rotateListInsertNewVal(&_list_FrontRight, receivedUltrasonicStruct.tFrontRight.f32Value);
            rotateListInsertNewVal(&_list_FrontCenterRight, receivedUltrasonicStruct.tFrontCenterRight.f32Value);

            UltrasonicStruct filteredUltrasonicStruct;
            filteredUltrasonicStruct.tFrontCenter.f32Value = sortCopyMedian(_list_FrontCenter);
            filteredUltrasonicStruct.tFrontCenterLeft.f32Value = sortCopyMedian(_list_FrontCenterLeft);
            filteredUltrasonicStruct.tFrontLeft.f32Value = sortCopyMedian(_list_FrontLeft);
            filteredUltrasonicStruct.tSideLeft.f32Value = sortCopyMedian(_list_SideLeft);
            filteredUltrasonicStruct.tRearLeft.f32Value = sortCopyMedian(_list_RearLeft);
            filteredUltrasonicStruct.tRearCenter.f32Value = sortCopyMedian(_list_RearCenter);
            filteredUltrasonicStruct.tRearRight.f32Value = sortCopyMedian(_list_RearRight);
            filteredUltrasonicStruct.tSideRight.f32Value = sortCopyMedian(_list_SideRight);
            filteredUltrasonicStruct.tFrontRight.f32Value = sortCopyMedian(_list_FrontRight);
            filteredUltrasonicStruct.tFrontCenterRight.f32Value = sortCopyMedian(_list_FrontCenterRight);
            sendData<UltrasonicStruct>(m_oOutputPin, &filteredUltrasonicStruct);
        }
    }
    RETURN_NOERROR;
}

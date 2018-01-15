#include "stdafx.h"
#include "medianFilter.h"

ADTF_FILTER_PLUGIN("medianFilter", OID_ADTF_MEDIAN_FILTER, cMedianFilter);

cMedianFilter::cMedianFilter(const tChar* __info):cFilter(__info) {
    SetPropertyInt("list_length", 10);
    SetPropertyStr("list_length" NSSUBPROP_DESCRIPTION, "length of the median filter list");
    SetPropertyInt("list_length" NSSUBPROP_MIN, 1);
    SetPropertyInt("list_length" NSSUBPROP_MAX, 1000);
    SetPropertyInt("initvalue", 400);
    medianFilter.initMedianFilter((uint64_t) GetPropertyInt("list_length"), GetPropertyInt("initvalue"));
}

cMedianFilter::~cMedianFilter() {
}

tResult cMedianFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))
    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pType;
        RETURN_IF_FAILED(AllocMediaType(&pType, MEDIA_TYPE_FILTERVALUE, MEDIA_SUBTYPE_FILTERVALUE, __exception_ptr));
        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin.Create("filterValue", pType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));
        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin.Create("filteredValue", pType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));
    }
    RETURN_NOERROR;
}

tResult cMedianFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cMedianFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        if (pSource == &m_oInputPin) {
            FilterValue filterValue = medianFilter.putNewValue(receiveData<FilterValue>(pMediaSample));
            sendData<FilterValue>(&m_oOutputPin, &filterValue);
        }
    }
    RETURN_NOERROR;
}
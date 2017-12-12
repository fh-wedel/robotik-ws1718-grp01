

#include "stdafx.h"
#include "medianFilter.h"
#include "../../protocol.h"

ADTF_FILTER_PLUGIN("medianFilter", OID_ADTF_MEDIAN_FILTER, cMedianFilter);


cMedianFilter::cMedianFilter(const tChar* __info):cFilter(__info) {
    //Property fuer die Median-Listen-Laenge
    SetPropertyInt("list_length", 10);
    SetPropertyStr("list_length" NSSUBPROP_DESCRIPTION, "length of the median filter list");
    SetPropertyInt("list_length" NSSUBPROP_MIN, 1);
    SetPropertyInt("list_length" NSSUBPROP_MAX, 1000);
    SetPropertyInt("initvalue", 400);
}

cMedianFilter::~cMedianFilter() {
}

tResult cMedianFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    //erzeugen der Liste mit der Laenge aus dem Property 'list_length'
    _list.resize((unsigned int)GetPropertyInt("list_length"));

    for (unsigned int i = 0; i < _list.size(); ++i) {
        _list[i] = GetPropertyInt("initvalue");
    }

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
            rotate(_list.begin(), _list.begin() + _list.size() - 1, _list.end());
            _list[0] = receiveData<FilterValue>(pMediaSample);

            vector<FilterValue> tmpList = _list;
            sort(tmpList.begin(), tmpList.end());
            sendData<FilterValue>(&m_oOutputPin, &(tmpList[tmpList.size()/2]));
        }
    }
    RETURN_NOERROR;
}

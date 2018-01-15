

#include "stdafx.h"
#include "negativNumberFilter.h"
#include "../../helperFunctions.h"

ADTF_FILTER_PLUGIN("negativNumberFilter", OID_ADTF_NEGATIVNUMBER_FILTER, cNegativNumberFilter);


cNegativNumberFilter::cNegativNumberFilter(const tChar* __info):cFilter(__info) {
}

cNegativNumberFilter::~cNegativNumberFilter() {
}

tResult cNegativNumberFilter::Init(tInitStage eStage, __exception) {
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

tResult cNegativNumberFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cNegativNumberFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oInputPin) {
            FilterValue f = receiveData<FilterValue>(pMediaSample);
            if (f > (0 - SIG_ATOMIC_MAX)) {
                f = fabs(f);
                sendData<FilterValue>(&m_oOutputPin, &(f));
            }
        }
    }
    RETURN_NOERROR;
}

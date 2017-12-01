

#include "stdafx.h"
#include "cTestBildFilter.h"


ADTF_FILTER_PLUGIN("testBildFilter", OID_ADTF_TESTBILDFILTER, cTestBildFilter);


cTestBildFilter::cTestBildFilter(const tChar* __info):cFilter(__info) {

}

cTestBildFilter::~cTestBildFilter() {
}

tResult cTestBildFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst) {

        // get a media type for the input pin
        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_VIDEO, MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin.Create("video_in", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_VIDEO, MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin.Create("video_out", pOutputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin));
    }

    RETURN_NOERROR;
}

tResult cTestBildFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}



tResult cTestBildFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        if (pSource == &m_oInputPin) {
            cout << "daten <--" << endl;
            _saveImage = receiveData<Mat>(pMediaSample);
            sendData<Mat>(m_oOutputPin, &_saveImage);
        }
    }
    RETURN_NOERROR;
}

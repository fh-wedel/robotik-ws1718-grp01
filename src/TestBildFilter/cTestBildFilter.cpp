

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

        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

        // Video Input
        RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));


        // Video Output
        RETURN_IF_FAILED(m_oVideoOutputPin.Create("Video_Output", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));
    }

    RETURN_NOERROR;
}

tResult cTestBildFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}



tResult cTestBildFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);
        if (pSource == &m_oVideoInputPin) {
            cout << "daten <--" << endl;

            Mat saveImage;
            saveImage = receiveData<Mat>(pMediaSample);

            sendData<Mat>(m_oVideoOutputPin, &saveImage);
            //newImage.Create(m_oVideoOutputPin.GetFormat(), NULL, saveImage.data);


            //sendData<cImage>(m_oVideoOutputPin, &newImage);
            /*cImage newImage;

            RETURN_IF_FAILED(pMediaSample->Update(_clock->GetStreamTime(), newImage.GetBitmap(), newImage.GetSize(), IMediaSample::MSF_None));
            //transmitting
            RETURN_IF_FAILED(m_oVideoOutputPin.Transmit(pMediaSample));

            saveImage.release();*/
        }
    }
    RETURN_NOERROR;
}

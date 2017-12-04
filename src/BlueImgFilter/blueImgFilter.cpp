#include <iostream>


#include "stdafx.h"
#include "../../protocol.h"
#include "blueImgFilter.h"



ADTF_FILTER_PLUGIN("blueImgFilter", OID_ADTF_BLUEIMG_FILTER, cBlueImgFilter);


cBlueImgFilter::cBlueImgFilter(const tChar* __info):cFilter(__info) {

}

cBlueImgFilter::~cBlueImgFilter() {
}

tResult cBlueImgFilter::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst) {
        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

        // Video Input
        RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));
        cout << "VideoInputPin created." << endl;


        // Video Ouput
        RETURN_IF_FAILED(m_oVideoOutputPin.Create("Video_Output", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));
        cout << "VideoOutputPin created." << endl;

    }

    RETURN_NOERROR;
}

tResult cBlueImgFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cBlueImgFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);


        if (pSource == &m_oVideoInputPin) {
            Mat image = receiveData(&m_oVideoInputPin, pMediaSample);

            Mat dest(image.rows, image.cols, image.type());
            dest = image.clone();

            //inRange(image,Scalar(0,0,0), Scalar(0,0,0),dest);
            //cout << "color range in image created: from image to dest" << endl;

            sendData(&m_oVideoOutputPin, &dest);
        }
    }
    RETURN_NOERROR;
}

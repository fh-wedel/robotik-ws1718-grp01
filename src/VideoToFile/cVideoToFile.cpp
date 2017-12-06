

#include "stdafx.h"
#include "cVideoToFile.h"


ADTF_FILTER_PLUGIN("videoToFile", OID_ADTF_VIDEOTOFILE, cVideoToFile);


cVideoToFile::cVideoToFile(const tChar* __info):cFilter(__info) {
    SetPropertyStr("filename", "/home/aadc/ADTF/records/filename.avi");
    SetPropertyStr("filename" NSSUBPROP_DESCRIPTION, "use absolut path");
    SetPropertyInt("FPS", 30);
}

cVideoToFile::~cVideoToFile() {
}

tResult cVideoToFile::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst) {

        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

        RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));
    }
    RETURN_NOERROR;
}

tResult cVideoToFile::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cVideoToFile::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    RETURN_IF_POINTER_NULL(pMediaSample);
    RETURN_IF_POINTER_NULL(pSource);

    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oVideoInputPin) {
            m_inputImage = receiveData(&m_oVideoInputPin, pMediaSample);
            if (!m_videoWriter.isOpened()) {
                m_videoWriter.open(GetPropertyStr("filename"), CV_FOURCC('M', 'J', 'P', 'G'), GetPropertyInt("FPS"), m_inputImage.size());
            }

            //Andere Types mussen seperat noch eingefuegt werden
            if (m_inputImage.type() == CV_8UC1) { // grayscale image (one channel) to 3 channel BGR image
                cvtColor(m_inputImage, m_inputImage, CV_GRAY2BGR);
            }

            m_videoWriter.write(m_inputImage);

        }
    }
    RETURN_NOERROR;
}

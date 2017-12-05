

#include "stdafx.h"
#include "cVideoToFile.h"


ADTF_FILTER_PLUGIN("videoToFile", OID_ADTF_VIDEOTOFILE, cVideoToFile);


cVideoToFile::cVideoToFile(const tChar* __info):cFilter(__info) {
    SetPropertyStr("filename", "/home/aadc/ADTF/records/filename.avi");
    SetPropertyStr("filename" NSSUBPROP_DESCRIPTION, "use absolut path");
    SetPropertyInt("FPS", 60);
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
    } else if (eStage == StageGraphReady) {
        cObjectPtr<IMediaType> pType;
        RETURN_IF_FAILED(m_oVideoInputPin.GetMediaType(&pType));
        cObjectPtr<IMediaTypeVideo> pTypeVideo;
        RETURN_IF_FAILED(pType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&pTypeVideo));
        if (IS_FAILED(UpdateInputImageFormat(pTypeVideo->GetFormat()))) {
            LOG_ERROR("Invalid Input Format for this filter");
        }
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
            //check if video format is still unkown
            if (m_sInputFormat.nPixelFormat == IImage::PF_UNKNOWN) {
                RETURN_IF_FAILED(UpdateInputImageFormat(m_oVideoInputPin.GetFormat()));
            }

            ProcessVideo(pMediaSample);

            if (!m_videoWriter.isOpened()) {
                m_videoWriter.open(GetPropertyStr("filename"), CV_FOURCC('M', 'J', 'P', 'G'), GetPropertyInt("FPS"), Size(m_sInputFormat.nWidth, m_sInputFormat.nHeight));
            }
            m_videoWriter.write(m_inputImage);

        }
    } else if (nEventCode == IPinEventSink::PE_MediaTypeChanged) {
        if (pSource == &m_oVideoInputPin) {
            //the input format was changed, so the imageformat has to changed in this filter also
            RETURN_IF_FAILED(UpdateInputImageFormat(m_oVideoInputPin.GetFormat()));
        }
    }
    RETURN_NOERROR;
}

tResult cVideoToFile::ProcessVideo(IMediaSample* pSample) {
    RETURN_IF_POINTER_NULL(pSample);
    cv::Mat outputImage;
    const tVoid* l_pSrcBuffer;
    if (IS_OK(pSample->Lock(&l_pSrcBuffer)))
    {
        //convert to mat, be sure to select the right pixelformat
        if (tInt32(m_inputImage.total() * m_inputImage.elemSize()) == m_sInputFormat.nSize)
        {
            memcpy(m_inputImage.data, l_pSrcBuffer, size_t(m_sInputFormat.nSize));
        }
        pSample->Unlock(l_pSrcBuffer);
    }
    RETURN_NOERROR;
}

tResult cVideoToFile::UpdateInputImageFormat(const tBitmapFormat* pFormat) {
    if (pFormat != NULL)
    {
        //update member variable
        m_sInputFormat = (*pFormat);
        LOG_INFO(adtf_util::cString::Format("Input: Size %d x %d ; BPL %d ; Size %d , PixelFormat; %d", m_sInputFormat.nWidth, m_sInputFormat.nHeight, m_sInputFormat.nBytesPerLine, m_sInputFormat.nSize, m_sInputFormat.nPixelFormat));
        //create the input matrix
        RETURN_IF_FAILED(BmpFormat2Mat(m_sInputFormat, m_inputImage));
    }
    RETURN_NOERROR;
}

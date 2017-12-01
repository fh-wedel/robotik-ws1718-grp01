#include <iostream>
#include <stdio.h>

//#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "stdafx.h"
#include "blueImgFilter.h"

#include "../../protocol.h"
//#include <opencv2/core/mat.hpp>

cv::Mat dgfhgjhjk();

ADTF_FILTER_PLUGIN("blueImgFilter", OID_ADTF_BLUEIMG_FILTER, cBlueImgFilter);


cBlueImgFilter::cBlueImgFilter(const tChar* __info):cFilter(__info) {

}

cBlueImgFilter::~cBlueImgFilter() {
}

tResult cBlueImgFilter::Init(tInitStage eStage, __exception) {
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

tResult cBlueImgFilter::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

tResult cBlueImgFilter::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);


        if (pSource == &m_oInputPin) {
            Mat image = receiveData<Mat>(pMediaSample);
            Mat filteredImage;

            // cv::Mat image(200,200,CV_8UC3,cv::Scalar(0,0,100));
           // cv::imshow("Fenstername ImgShow", image);

            //image = receiveData<cv::Mat>(pMediaSample);
            //cv::imshow("Fenstername", mtrx);



            cout << "Daten empfangen." << endl;

        }
    }
    RETURN_NOERROR;
}

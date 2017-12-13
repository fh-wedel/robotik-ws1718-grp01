#include <iostream>


#include "stdafx.h"
#include "../../protocol.h"
#include "oneLineDetect.h"



ADTF_FILTER_PLUGIN("oneLineDetect", OID_ADTF_OneLineDetect_FILTER, cOneLineDetect);


cOneLineDetect::cOneLineDetect(const tChar* __info):cFilter(__info) {
    SetPropertyInt("minLineWidth", 10);
}

cOneLineDetect::~cOneLineDetect() {
}

tResult cOneLineDetect::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst) {
        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

        // Video Input
        RETURN_IF_FAILED(m_oVideoInputPin.Create("Video_Input", IPin::PD_Input, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoInputPin));
        cout << "VideoInputPin created." << endl;


        // Video Ouput
        RETURN_IF_FAILED(m_oVideoOutputPin.Create("Debug_Video", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));
        cout << "VideoOutputPin created." << endl;

        cObjectPtr<IMediaType> pInputType;
        RETURN_IF_FAILED(AllocMediaType(&pInputType, MEDIA_TYPE_LINEDETECTION, MEDIA_SUBTYPE_LINEDETECTIONDIFF, __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oDiff_CenterPin.Create("Diff_Center", pInputType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oDiff_CenterPin));


    }

    RETURN_NOERROR;
}

tResult cOneLineDetect::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}


// Differenz zum bildmittelpunt der Zeile ausgeben und Markierung im Debugvideo setzen
int cOneLineDetect::whiteAreaInRow(int targetRow, Mat src, Mat greyImg){
    int mid = greyImg.cols/2;

    int i;
    int pos = 0;
    int cnt = 0;
    int max = 0;

    // Groesste zusammen haengende Anzahl an weißen Pixeln finden (Markierung detektieren)
    for(i = 0; i < greyImg.cols; i++) {

        if (greyImg.at<uchar>(greyImg.rows - targetRow, i) == 255) {

            cnt++;

            if (cnt >= max) {
                max = cnt;
                pos = i;
            }
        } else {
            cnt = 0;
        }
    }

    // Linienosition relativ zur Bildmitte ermitteln
    pos = pos - (max/2);

    //erkannte Linie markieren
    Point pt(pos, src.rows - targetRow);


    int offset =  (int) (pos <= mid) ? - (mid - pos) : (pos - mid);
    //return offset;
    if (max < GetPropertyInt("minLineWidth") ) {
        return -101;
    }
    circle(src, pt, 5, Scalar(0, 0, 255), 3, 0, 0);
    return (int) (offset <= -320 || offset >= 320) ? -101 : offset*100/320; // Normieren des Outputs auf (-100 <= returned_offset <= 100)
}

tResult cOneLineDetect::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);


        if (pSource == &m_oVideoInputPin) {
            Mat image = receiveData(&m_oVideoInputPin, pMediaSample);
            Mat greyImg;

            cvtColor(image, greyImg, CV_BGR2GRAY);


            LineDetectionDiff difference_0 = cOneLineDetect::whiteAreaInRow(15, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_0);
            //cout << "Difference: " << difference_0 << endl;

            /*
            LineDetectionDiff difference_0 = cOneLineDetect::whiteAreaInRow(1, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_0);

            LineDetectionDiff difference_1 = cOneLineDetect::whiteAreaInRow(11, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_1);

            LineDetectionDiff difference_2 = cOneLineDetect::whiteAreaInRow(21, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_2);

            LineDetectionDiff difference_3 = cOneLineDetect::whiteAreaInRow(31, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_3);

            LineDetectionDiff difference_4 = cOneLineDetect::whiteAreaInRow(41, image, greyImg);
            sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_4);
             */

            sendData(&m_oVideoOutputPin, &image);
        }
    }
    RETURN_NOERROR;
}





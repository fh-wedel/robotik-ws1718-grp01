#include <iostream>


#include "stdafx.h"
#include "../../helperFunctions.h"
#include "oneLineDetect.h"



ADTF_FILTER_PLUGIN("oneLineDetect", OID_ADTF_OneLineDetect_FILTER, cOneLineDetect);


cOneLineDetect::cOneLineDetect(const tChar* __info):cFilter(__info) {
    SetPropertyInt("minLineWidth", 10);
    SetPropertyInt("maxLineWidth", 60);

    medianFilter.initMedianFilter((uint64_t) GetPropertyInt("list_length"), GetPropertyInt("initvalue"));

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

// Differenz zum Bildmittelpunt der Zeile ausgeben und Markierung im Debugvideo setzen
int cOneLineDetect::doubleWhiteAreaInRow(int targetRow, Mat src, Mat greyImg){
    int mid = greyImg.cols/2;

    // position der Fahrlinienmitte
    int pos = 0;

    int i;
    int cnt = 0;

    int maxpos0 = 0;
    int maxwidth0 = 0;

    // Groesste zusammen haengende Anzahl an weißen Pixeln finden (Markierung detektieren)
    for(i = 0; i < greyImg.cols; i++) {

        if (greyImg.at<uchar>(greyImg.rows - targetRow, i) == 255) {
            // if pixel is white
            cnt++;

            if (cnt >= maxwidth0) {
                maxwidth0 = cnt;
                maxpos0 = i;
            }

        } else {
            cnt = 0;
        }
    }

    if(maxwidth0 > GetPropertyInt("maxLineWidth") ) {
        pos = -101;
    }

    // get second largest line in row
    int maxpos1 = 0;
    int maxwidth1 = 0;

    for(i = 0; i < greyImg.cols; i++) {

        if(i <= maxpos0 && i >= maxpos0-maxwidth0) {
            if (greyImg.at<uchar>(greyImg.rows - targetRow, i) == 255) {
                // if pixel is white
                cnt++;

                if (cnt >= maxwidth0) {
                    maxwidth1 = cnt;
                    maxpos1 = i;
                }

            } else {
                cnt = 0;
            }
        }
    }

    if(maxwidth1 < GetPropertyInt("maxLineWidth") ) {
        pos =  maxpos0 - (maxwidth0/2);
    } else {
        // decide which of the two recognized lines to return

        //return maxpos0 - (maxwidth0/2); // always return wider line

        // wähle Linie, die zentraler liegt
        int center0diff = fabs( mid - (maxpos0 - (maxwidth0/2)));
        int center1diff = fabs( mid - (maxpos1 - (maxwidth1/2)));
        pos =  center0diff < center1diff ? center0diff : center1diff;
        // ...

    }

    //erkannte Linie markieren
    Point pt(pos, src.rows - targetRow);


    int offset =  (int) (pos <= mid) ? - (mid - pos) : (pos - mid);
    //return offset;
    /*
    if (max < GetPropertyInt("minLineWidth") ) {
        return -101;
    }
     */
    circle(src, pt, 5, Scalar(0, 0, 255), 3, 0, 0);
    return (int) (offset <= -320 || offset >= 320) ? -101 : offset*100/320; // Normieren des Outputs auf (-100 <= returned_offset <= 100)
}


// Differenz zum Bildmittelpunt der Zeile ausgeben und Markierung im Debugvideo setzen
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

            // Berechnet den Median-Wert aus zehn Pixel-Reihen-Werten im oberen Bildbereich
            vector<FilterValue> tmpFilterList_1;
            //for (unsigned int i = 130; i <150; i+=2){
			for (unsigned int i = 130; i <150; i+=2){
                //tmpFilterList_1.push_back( cOneLineDetect::whiteAreaInRow(i, image, greyImg));
                tmpFilterList_1.push_back( doubleWhiteAreaInRow(i, image, greyImg));
            }
            LineDetectionDiff difference_1 = medianFilter.medianFromArray(tmpFilterList_1);



            if (difference_1 == -101) {
                // Berechnet den Median-Wert aus zehn Pixel-Reihen-Werten im unteren Bildbereich
                vector<FilterValue> tmpFilterList_0;
                for (unsigned int i = 30; i < 50; i+=2){
                    //tmpFilterList_0.push_back( cOneLineDetect::whiteAreaInRow(i, image, greyImg));
                    tmpFilterList_0.push_back( doubleWhiteAreaInRow(i, image, greyImg));
                }
                LineDetectionDiff difference_0 = medianFilter.medianFromArray(tmpFilterList_0);

                sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_0);
            } else {
                sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &difference_1);
            }


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





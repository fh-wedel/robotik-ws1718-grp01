#include <iostream>


#include "stdafx.h"
#include "../../helperFunctions.h"
#include "oneLineDetect.h"



ADTF_FILTER_PLUGIN("oneLineDetect", OID_ADTF_OneLineDetect_FILTER, cOneLineDetect);


cOneLineDetect::cOneLineDetect(const tChar* __info):cFilter(__info) {
    SetPropertyInt("minLineWidth", 10);
    SetPropertyInt("maxLineWidth", 80);

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

/**
 * Sucht nach den zwei größten weißen Pixelfolgen im mitgegebenen Schwarzweißbild
 * und wählt den Mittelpunkt des relevanteren der beiden.
 * @param targetRow die Reihe (von unten gezählt), in der gesucht werden soll
 * @param src das ungefilterete Ursprungsbild, in welchem der detektierte Punkt markiert werden soll
 * @param greyImg das gefilterte Schwarzweißbild, in welchem gesucht werden soll
 * @return die Position der berechneten Fahrstreifenmitte im Bereich [-100,100]
 */
int cOneLineDetect::doubleWhiteAreaInRow(int targetRow, Mat src, Mat greyImg){
    int mid_img = greyImg.cols/2;

    int row = greyImg.rows - targetRow;

    int currwidth = 0; // Aktuelle Weißpixelfolgen-Länge
    int maxwidth1 = 0; // Bislang längste Weißpixelfolgen-Länge
    int endpos_maxwidth1 = 0; // Endposition der bislang längsten Weißpixelfolge

    // Groesste zusammenhaengende Anzahl an weißen Pixeln finden
    for(int i = 0; i < greyImg.cols; i++) {

        if (greyImg.at<uchar>(row, i) != 255) {
            currwidth = 0;

        } else { // white pixel detected
            currwidth++;

            if (currwidth >= maxwidth1) {
                maxwidth1 = currwidth;
                endpos_maxwidth1 = i;
            }
        }
    }

    if (maxwidth1 < GetPropertyInt("minLineWidth") || maxwidth1 > GetPropertyInt("maxLineWidth")) {
        return -101;
    }

    // get second largest line in row
    int beginpos_maxwidth1 = endpos_maxwidth1 - maxwidth1;
    currwidth = 0;

    int maxwidth2 = 0;
    int endpos_maxwidth2 = 0;

    for(int i = 0; i < greyImg.cols; i++) {

        if(i < beginpos_maxwidth1 || i > endpos_maxwidth1) {

            if (greyImg.at<uchar>(greyImg.rows - targetRow, i) != 255) {
                currwidth = 0;

            } else { // white pixel detected
                currwidth++;

                if (currwidth >= maxwidth2) {
                    maxwidth2 = currwidth;
                    endpos_maxwidth2 = i;
                }

            }
        }
    }


    // Linienmittelpunkt bestimmen -
    int linecenter = endpos_maxwidth1 - (maxwidth1/2);

    if (maxwidth2 >= GetPropertyInt("minLineWidth") && maxwidth2 <= GetPropertyInt("maxLineWidth")) {
        // Wahl, welche Linie genutzt werden woll

        // wähle immer die breitere Linie
        //return endpos_maxwidth1 - (maxwidth1/2);

        // wähle Linie, die zentraler liegt
        int center1 = endpos_maxwidth1 - (maxwidth1/2);
        int center1_middist = fabs( mid_img - center1 );

        int center2 = endpos_maxwidth2 - (maxwidth2/2);
        int center2_middist = fabs( mid_img - center2);

        linecenter =  center1_middist < center2_middist ? center1 : center2;
        // ...
    }


    // Punkt relativ zum zum Bildmittelpunkt (auf 0 gesetzt) berechnen
    int offset = (int) linecenter - mid_img;

    // erkannten Linienmittelpunkt im Ursprungsbild markieren
    Point pt(linecenter, src.rows - targetRow);
    circle(src, pt, 5, Scalar(0, 0, 255), 3, 0, 0);

    if (offset <= -320 || offset >= 320){
        return -101;
    }
    // Normieren des Outputs auf {-100 <= returned_offset <= 100}
    return offset*100/320;
}


/**
 * Sucht nach der größten weißen Pixelfolge im mitgegebenen Schwarzweißbild
 * und wählt den Mittelpunkt des relevanteren der beiden.
 * @param targetRow die Reihe (von unten gezählt), in der gesucht werden soll
 * @param src das ungefilterete Ursprungsbild, in welchem der detektierte Punkt markiert werden soll
 * @param greyImg das gefilterte Schwarzweißbild, in welchem gesucht werden soll
 * @return die Position der berechneten Fahrstreifenmitte im Bereich [-100,100]
 */
int cOneLineDetect::whiteAreaInRow(int targetRow, Mat src, Mat greyImg){
    int mid_img = greyImg.cols/2;

    int row = greyImg.rows - targetRow;

    int currwidth = 0; // Aktuelle Weißpixelfolgen-Länge
    int maxwidth = 0; // Bislang längste Weißpixelfolgen-Länge
    int endpos_maxwidth = 0; // Endposition der bislang längsten Weißpixelfolge

    // Groesste zusammenhaengende Anzahl an weißen Pixeln finden
    for(int i = 0; i < greyImg.cols; i++) {

        if (greyImg.at<uchar>(row, i) == 255) {

            currwidth++;

            if (currwidth >= maxwidth) {
                maxwidth = currwidth;
                endpos_maxwidth = i;
            }

        } else {
            currwidth = 0;
        }
    }

    if (maxwidth < GetPropertyInt("minLineWidth") || maxwidth > GetPropertyInt("maxLineWidth")) {
        return -101;
    }

    // Linienmittelpunkt bestimmen
    int linecenter =  endpos_maxwidth - (maxwidth/2);

    // Punkt relativ zum zum Bildmittelpunkt (auf 0 gesetzt) berechnen
    int offset = (int) linecenter - mid_img;

    // erkannten Linienmittelpunkt im Ursprungsbild markieren
    Point pt(linecenter, src.rows - targetRow);
    circle(src, pt, 5, Scalar(0, 0, 255), 3, 0, 0);

    if (offset <= -320 || offset >= 320){
        return -101;
    }
    // Normieren des Outputs auf {-100 <= returned_offset <= 100}
    return offset*100/320;
}


tResult cOneLineDetect::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        if (pSource == &m_oVideoInputPin) {
            Mat image = receiveData(&m_oVideoInputPin, pMediaSample);
            Mat greyImg;

            //übersetzt das 'image' vom Colorspace BGR in GRAY und schreibt das Ergebnis in 'greyImg'
            cvtColor(image, greyImg, CV_BGR2GRAY);

            // Berechnet den Median-Wert aus zehn Pixel-Reihen-Werten im oberen Bildbereich
            vector<FilterValue> tmpFilterList_1;
			for (unsigned int i = 130; i <150; i+=2){
                tmpFilterList_1.push_back( cOneLineDetect::whiteAreaInRow(i, image, greyImg));
                //tmpFilterList_1.push_back( cOneLineDetect::doubleWhiteAreaInRow(i, image, greyImg));
            }
            LineDetectionDiff foundline_higher = medianFilter.medianFromArray(tmpFilterList_1);


            if (foundline_higher != -101) {
                sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &foundline_higher);

            } else {
                // Berechnet den Median-Wert aus zehn Pixel-Reihen-Werten im unteren Bildbereich
                vector<FilterValue> tmpFilterList_0;
                for (unsigned int i = 30; i < 50; i+=2){
                    tmpFilterList_0.push_back( cOneLineDetect::whiteAreaInRow(i, image, greyImg));
                    //tmpFilterList_0.push_back( cOneLineDetect::doubleWhiteAreaInRow(i, image, greyImg));
                }
                LineDetectionDiff foundline_lower = medianFilter.medianFromArray(tmpFilterList_0);

                sendData<LineDetectionDiff>(&m_oDiff_CenterPin, &foundline_lower);
            }

            sendData(&m_oVideoOutputPin, &image);
        }
    }
    RETURN_NOERROR;
}





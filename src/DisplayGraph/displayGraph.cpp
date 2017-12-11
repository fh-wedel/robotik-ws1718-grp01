

#include "stdafx.h"
#include "displayGraph.h"
#include "../../protocol.h"
#include <iostream>

ADTF_FILTER_PLUGIN("displayGraph", OID_ADTF_DisplayGraph, cDisplayGraph);


cDisplayGraph::cDisplayGraph(const tChar* __info):cFilter(__info) {
    SetPropertyInt("width", 400);
    SetPropertyInt("height", 200);
    SetPropertyInt("rangeMax", 500);
    SetPropertyInt("rangeMin", -10);
}

cDisplayGraph::~cDisplayGraph() {
}

tResult cDisplayGraph::Init(tInitStage eStage, __exception) {
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    _list.resize(2);

    _graphSize.rangeMin = GetPropertyInt("rangeMin");
    _graphSize.rangeMax = GetPropertyInt("rangeMax");
    _graphSize.heightPx = GetPropertyInt("height");
    _graphSize.widthPx = GetPropertyInt("width");

    //_image = Mat::ones(_graphSize.heightPx, _graphSize.widthPx, CV_8UC3);

    _image = Mat(_graphSize.heightPx, _graphSize.widthPx, CV_8UC3, Vec3b(255,255,255));

    if (eStage == StageFirst) {
        // get a media type for the input pin
        cObjectPtr<IMediaType> pType;
        RETURN_IF_FAILED(AllocMediaType(&pType, MEDIA_TYPE_FILTERVALUE, MEDIA_SUBTYPE_FILTERVALUE, __exception_ptr));
        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin1.Create("value1", pType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin1));

        RETURN_IF_FAILED(m_oInputPin2.Create("value2", pType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin2));

        RETURN_IF_FAILED(m_oVideoOutputPin.Create("Video_Output", IPin::PD_Output, static_cast<IPinEventSink*>(this)));
        RETURN_IF_FAILED(RegisterPin(&m_oVideoOutputPin));
    }
    RETURN_NOERROR;
}

tResult cDisplayGraph::Shutdown(tInitStage eStage, __exception) {
    return cFilter::Shutdown(eStage, __exception_ptr);
}

int cDisplayGraph::map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

tResult cDisplayGraph::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample) {
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        RETURN_IF_POINTER_NULL(pMediaSample);

        uint8_t tmpBittest = 0;
        if (pSource == &m_oInputPin1) {
            _list[0] = receiveData<FilterValue>(pMediaSample);
            tmpBittest |= 1;
        } else if (pSource == &m_oInputPin2) {
            _list[1] = receiveData<FilterValue>(pMediaSample);
            tmpBittest |= 2;
        }

        if (((_bitTest | tmpBittest) == 3) || ((_bitTest & tmpBittest) != 0)) {
            _bitTest = 0;

            Mat tmp = _image.clone();

            _image = Mat(_graphSize.heightPx, _graphSize.widthPx, _image.type(), Vec3b(255,255,255));

            tmp(Rect(1,0,tmp.cols-1,tmp.rows)).copyTo(_image(Rect(0,0,_image.cols-1, _image.rows)));

            _image.at<Vec3b>(map(0, _graphSize.rangeMin, _graphSize.rangeMax, _graphSize.heightPx, 0),_image.cols-1) = Vec3b(0,0,0);


            _image.at<Vec3b>(map(_list[0], _graphSize.rangeMin, _graphSize.rangeMax, _graphSize.heightPx, 0),_image.cols-1) = Vec3b(0,0,255);
            _image.at<Vec3b>(map(_list[1], _graphSize.rangeMin, _graphSize.rangeMax, _graphSize.heightPx, 0),_image.cols-1) = Vec3b(0,255,0);

            sendData(&m_oVideoOutputPin, &_image);

        } else {
            _bitTest |= tmpBittest;
        }
    }
    RETURN_NOERROR;
}

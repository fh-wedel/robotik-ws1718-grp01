#ifndef _displayGraph_H_
#define _displayGraph_H_

#define OID_ADTF_DisplayGraph "adtf.example.displayGraph"

#include "../../protocol.h"

typedef struct {
    int heightPx;
    int widthPx;
    int rangeMax;
    int rangeMin;
} GraphSize;

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cDisplayGraph : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_DisplayGraph, "displayGraph", adtf::OBJCAT_DataFilter);

private:
    vector<float> _list;
    uint8_t _bitTest;
    Mat _image;
    GraphSize _graphSize;
    int map(long x, long in_min, long in_max, long out_min, long out_max);

protected:
    cInputPin m_oInputPin1;
    cInputPin m_oInputPin2;
    cVideoPin m_oVideoOutputPin;

public:
    cDisplayGraph(const tChar* __info);
    virtual ~cDisplayGraph();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


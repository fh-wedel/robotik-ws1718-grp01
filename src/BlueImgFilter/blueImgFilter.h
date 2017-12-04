#ifndef _blueImgFilter_H_
#define _blueImgFilter_H_

#define OID_ADTF_BLUEIMG_FILTER "adtf.example.blueImgFilter"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cBlueImgFilter : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_BLUEIMG_FILTER, "blueImgFilter", adtf::OBJCAT_DataFilter);

private:

protected:
    cVideoPin m_oVideoInputPin;
    cVideoPin m_oVideoOutputPin;

public:
    cBlueImgFilter(const tChar* __info);
    virtual ~cBlueImgFilter();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


#ifndef _ultrasonicSplit_H_
#define _ultrasonicSplit_H_

#define OID_ADTF_ULTRASONICSPLIT "adtf.example.ultrasonicSplit"

/*
* MedianFilter zum Filtern der Ultraschallsensoren.
*/
class cUltrasonicSplit : public adtf::cFilter {
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_ULTRASONICSPLIT, "ultrasonicSplit", adtf::OBJCAT_DataFilter);


protected:
    cInputPin m_oInputPin;

    cOutputPin m_oOutputPinFrontCenter;
    cOutputPin m_oOutputPinFrontCenterLeft;
    cOutputPin m_oOutputPinFrontLeft;
    cOutputPin m_oOutputPinSideLeft;
    cOutputPin m_oOutputPinRearLeft;
    cOutputPin m_oOutputPinRearCenter;
    cOutputPin m_oOutputPinSideRight;
    cOutputPin m_oOutputPinRearRight;
    cOutputPin m_oOutputPinFrontRight;
    cOutputPin m_oOutputPinFrontCenterRight;




public:
    cUltrasonicSplit(const tChar* __info);
    virtual ~cUltrasonicSplit();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _medianFilter_H_


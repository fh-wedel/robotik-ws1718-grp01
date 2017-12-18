
#ifndef _ACCELERATION_FILTER_H_
#define _ACCELERATION_FILTER_H_

#define OID_ADTF_ACCELERATION_FILTER "adtf.example.accelerationFilter"


//!  Template filter for AADC Teams
/*!
* This is a example filter for the AADC
*/
class c_accelerationFilter : public adtf::cFilter
{
    /*! set the filter ID and the version */
    ADTF_FILTER(OID_ADTF_ACCELERATION_FILTER, "AccelerationFilter", adtf::OBJCAT_DataFilter);

#define CALIBRATION 1
#define CALIBRATION_SAMPLES 60


#define G 9.80665
#define ACC_Z_NORMAL 9.75
#define ACC_Z_DELTA_NEG 0.02
#define ACC_Z_DELTA_POS 0.01


#define ACC_Y_NORMAL -0.85
#define ACC_X_NORMAL 0.13
protected:
    cInputPin    m_oInputPin_acceleration;

    cOutputPin    m_oOutputPin_flags;

public:
    c_accelerationFilter(const tChar* __info);

    virtual ~c_accelerationFilter();

protected:
    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);

    tResult TransmitFloatValue(cOutputPin* oPin, tFloat32 value, tUInt32 timestamp);

};



#endif // _ACCELERATION_FILTER_H_

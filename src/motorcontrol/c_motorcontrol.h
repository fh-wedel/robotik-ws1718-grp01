
#ifndef _MOTORCONTROL_H_
#define _MOTORCONTROL_H_

#define OID_ADTF_TEMPLATE_FILTER "adtf.example.motorcontrol"


class c_motorcontrol : public adtf::cFilter
{
    ADTF_FILTER(OID_ADTF_TEMPLATE_FILTER, "Motorcontol", adtf::OBJCAT_DataFilter);

#define CALIBRATION 1
#define CALIBRATION_SAMPLES 60
#define MAX_SPEED -12

#define G 9.80665
#define ACC_Z_NORMAL 9.75
#define ACC_Z_DELTA_NEG 0.02
#define ACC_Z_DELTA_POS 0.01


#define ACC_Y_NORMAL -0.85
#define ACC_X_NORMAL 0.13

protected:
    cInputPin    m_oInputPin_motorcontol;

    cInputPin    m_oInputPin_flags;

    cOutputPin    m_oOutputPin_speed;

    cOutputPin    m_oOutputPin_angle;


public:

    c_motorcontrol(const tChar* __info);

    virtual ~c_motorcontrol();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);

private:
    void emergeny_break();
};
#endif // _MOTORCONTROL_H_

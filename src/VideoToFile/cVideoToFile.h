#ifndef _VIDEOTOFILE_H_
#define _VIDEOTOFILE_H_


#include <aadc_structs.h>
#include <iostream>
#include "../../protocol.h"

#define OID_ADTF_VIDEOTOFILE "adtf.example.videoToFile"

class cVideoToFile : public adtf::cFilter {
    ADTF_FILTER(OID_ADTF_VIDEOTOFILE, "videoToFile", adtf::OBJCAT_DataFilter);

private:
    /*! tha last received input image*/
    Mat m_inputImage;

    VideoWriter m_videoWriter;

protected:
    cVideoPin m_oVideoInputPin;

public:
    cVideoToFile(const tChar* __info);
    virtual ~cVideoToFile();

protected:

    tResult Init(tInitStage eStage, ucom::IException** __exception_ptr);

    tResult Shutdown(tInitStage eStage, ucom::IException** __exception_ptr = NULL);

    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _TESTBILDFILTER_H_


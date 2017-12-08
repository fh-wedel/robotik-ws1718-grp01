/**
Copyright (c)
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2017-05-12 09:34:53#$ $Rev:: 63109   $
**********************************************************************/


#include "stdafx.h"
#include "c_controller.h"
#include "../../protocol.h"
#include <stdio.h>
#include <cmath>



/// Create filter shell
ADTF_FILTER_PLUGIN("Controller", OID_ADTF_TEMPLATE_FILTER, c_controller);


tFloat32 sum_x = 0;
tFloat32 sum_y = 0;
tFloat32 sum_z = 0;
tInt32 samples = 0;

float cur_speed = 0;
float cur_angle = 0;
bool emergeny_break_enabled = 0;


c_controller::c_controller(const tChar *__info) : cFilter(__info) {

}

c_controller::~c_controller() {

}


tResult c_controller::Init(tInitStage eStage, __exception) {
    // never miss calling the parent implementation!!
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    // in StageFirst you can create and register your static pins.
    if (eStage == StageFirst) {
		
		
		cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER, IID_ADTF_MEDIA_DESCRIPTION_MANAGER, (tVoid**)&pDescManager, __exception_ptr));

    
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");
    RETURN_IF_POINTER_NULL(strDescSignalValue);
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionAccelerateSignalInput))
		
		
        // get a media type for the input pin
        cObjectPtr<IMediaType> pUSSType;
        RETURN_IF_FAILED(AllocMediaType(&pUSSType, MEDIA_TYPE_ULTRASONICSTRUCT, MEDIA_SUBTYPE_ULTRASONICSTRUCT,
                                        __exception_ptr));

        cObjectPtr<IMediaType> pdiffType;
        RETURN_IF_FAILED(AllocMediaType(&pdiffType, MEDIA_TYPE_LINEDETECTION, MEDIA_SUBTYPE_LINEDETECTIONDIFF, __exception_ptr));

		cObjectPtr<IMediaType> pSpeedType;
        RETURN_IF_FAILED(AllocMediaType(&pSpeedType, MEDIA_TYPE_OTHER, MEDIA_TYPE_OTHER,
                                        __exception_ptr));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_USS.Create("USS", pUSSType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_USS));

        // create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_diff.Create("diff", pdiffType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_diff));

// create and register the input pin
        RETURN_IF_FAILED(m_oInputPin_speed.Create("speed", pSpeedType, this));
        RETURN_IF_FAILED(RegisterPin(&m_oInputPin_speed));

        // get a media type for the output pin
        cObjectPtr<IMediaType> pOutputType;
        RETURN_IF_FAILED(AllocMediaType(&pOutputType, MEDIA_TYPE_MOTORCONTROL, MEDIA_SUBTYPE_MOTORCONTROL, __exception_ptr));

        // create and register the output pin
        RETURN_IF_FAILED(m_oOutputPin_carcontrol.Create("carcontrol", pOutputType, static_cast<IPinEventSink *> (this)));
        RETURN_IF_FAILED(RegisterPin(&m_oOutputPin_carcontrol));
    } else if (eStage == StageNormal) {
        // In this stage you would do further initialisation and/or create your dynamic pins.
        // Please take a look at the demo_dynamicpin example for further reference.
    } else if (eStage == StageGraphReady) {
        // All pin connections have been established in this stage so you can query your pins
        // about their media types and additional meta data.
        // Please take a look at the demo_imageproc example for further reference.
    }
    
    
    

    RETURN_NOERROR;
}

tResult c_controller::Shutdown(tInitStage eStage, __exception) {
    // In each stage clean up everything that you initiaized in the corresponding stage during Init.
    // Pins are an exception:
    // - The base class takes care of static pins that are members of this class.
    // - Dynamic pins have to be cleaned up in the ReleasePins method, please see the demo_dynamicpin
    //   example for further reference.

    if (eStage == StageGraphReady) {
    } else if (eStage == StageNormal) {
    } else if (eStage == StageFirst) {
    }

    // call the base class implementation
    return cFilter::Shutdown(eStage, __exception_ptr);
}


tResult c_controller::OnPinEvent(IPin *pSource,
                                   tInt nEventCode,
                                   tInt nParam1,
                                   tInt nParam2,
                                   IMediaSample *pMediaSample) {
    // first check what kind of event it is
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived) {
        if (pSource == &m_oInputPin_USS) {
		
		


        }
        if (pSource == &m_oInputPin_diff) {


            

        }
        
        if (pSource == &m_oInputPin_speed) {
		float f32value;
		static tBufferID szIDF32Value_SpeedContr;

		__adtf_sample_read_lock_mediadescription(m_pDescriptionAccelerateSignalInput, pMediaSample, pCoderInput);

        
            pCoderInput->GetID("f32Value", szIDF32Value_SpeedContr);
        

        pCoderInput->Get(szIDF32Value_SpeedContr, (tVoid*)&f32value);
          
          
        MotorControl test;
		test.speed = f32value;
		sendData<MotorControl>(&m_oOutputPin_carcontrol, &test);
           printf("start send");   




        }

    }

    RETURN_NOERROR;
}



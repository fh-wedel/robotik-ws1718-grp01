//
// Created by aadc on 20.11.17.
//

#ifndef PROJECT_PROTOCOL_H
#define PROJECT_PROTOCOL_H

#include <stdint.h>

#define valueToMessage(m) ((void*)(uint64_t)m)
#define messageToValue(v) ((uint64_t)v)

#define MEDIA_TYPE_Kalman_1D     0x00000000
#define MEDIA_SUBTYPE_Kalman_1D  0x00000000

#define MEDIA_TYPE_ULTRASONICSTRUCT     0x00000000
#define MEDIA_SUBTYPE_ULTRASONICSTRUCT  0x00000000



#endif //PROJECT_PROTOCOL_H

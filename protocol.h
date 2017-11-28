//
// Created by aadc on 20.11.17.
//

#ifndef PROJECT_PROTOCOL_H
#define PROJECT_PROTOCOL_H

#include <stdint.h>

#define valueToMessage(m) ((void*)(uint64_t)m)
#define messageToValue(v) ((uint64_t)v)

#define MEDIA_TYPE_ULTRASONICSTRUCT     0
#define MEDIA_SUBTYPE_ULTRASONICSTRUCT  0

#define MEDIA_TYPE_REQUEST              1
#define MEDIA_SUBTYPE_REQUEST           1

#define MEDIA_TYPE_FLAG                 2
#define MEDIA_SUBTYPE_FLAG              2


#endif //PROJECT_PROTOCOL_H

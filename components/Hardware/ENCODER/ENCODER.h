#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ENCODER_CC.h"
#include "ENCODER_PC.h"

int Encoder_Get_Count(encoder_id_t encoder_id);
float Encoder_Get_Speed(encoder_id_t encoder_id);

#endif


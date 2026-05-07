#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ENCODER_CC.h"
#include "ENCODER_PC.h"

void ENCODER_Init(void);
int Encoder_Get_Count_M1(void);
int Encoder_Get_Count_M2(void);
int Encoder_Get_Count_M3(void);
int Encoder_Get_Count_M4(void);
int Encoder_Get_Count(encoder_id_t encoder_id);

#endif


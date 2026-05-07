#include "ENCODER.h"
#include "BOARD.h"

static ESPENCODER_Class_t s_legacy_encoder;

static ENCODER_Base_t *ENCODER_Get_Default(void)
{
    if (Chassis_Encoder == NULL)
    {
        ESPENCODER_Init(&s_legacy_encoder, "chassis_encoder");
        Chassis_Encoder = &s_legacy_encoder.base;
    }

    return Chassis_Encoder;
}

void ENCODER_Init(void)
{
    ESPENCODER_Init(&s_legacy_encoder, "chassis_encoder");
    Chassis_Encoder = &s_legacy_encoder.base;
}

int Encoder_Get_Count_M1(void)
{
    return ENCODER_GetCount(ENCODER_Get_Default(), ENCODER_ID_M1);
}

int Encoder_Get_Count_M2(void)
{
    return ENCODER_GetCount(ENCODER_Get_Default(), ENCODER_ID_M2);
}

int Encoder_Get_Count_M3(void)
{
    return ENCODER_GetCount(ENCODER_Get_Default(), ENCODER_ID_M3);
}

int Encoder_Get_Count_M4(void)
{
    return ENCODER_GetCount(ENCODER_Get_Default(), ENCODER_ID_M4);
}

int Encoder_Get_Count(encoder_id_t encoder_id)
{
    return ENCODER_GetCount(ENCODER_Get_Default(), encoder_id);
}


#ifndef __ENCODER_PC_H__
#define __ENCODER_PC_H__

#include <assert.h>

typedef enum
{
    ENCODER_ID_M1 = 1,
    ENCODER_ID_M2 = 2,
    ENCODER_ID_M3 = 3,
    ENCODER_ID_M4 = 4,
} encoder_id_t;

typedef struct ENCODER_Base ENCODER_Base_t;

typedef struct
{
    int (*ENCODER_GET_COUNT)(ENCODER_Base_t *self, encoder_id_t encoder_id);
} ENCODER_Ops_t;

struct ENCODER_Base
{
    const char *name;
    const ENCODER_Ops_t *ops;
};

int ENCODER_GET_COUNT(ENCODER_Base_t *self, encoder_id_t encoder_id);
int ENCODER_GetCount(ENCODER_Base_t *self, encoder_id_t encoder_id);

#endif

#ifndef __BEEP_PC_H__
#define __BEEP_PC_H__

#include <assert.h>

typedef struct BEEP_Base BEEP_Base_t;

typedef struct
{
    void (*BEEP_On)(BEEP_Base_t *self);
    void (*BEEP_Off)(BEEP_Base_t *self);
} BEEP_Ops_t;

struct BEEP_Base
{
    const char *name;
    const BEEP_Ops_t *ops;
};

void BEEP_On(BEEP_Base_t *self);
void BEEP_Off(BEEP_Base_t *self);

#endif


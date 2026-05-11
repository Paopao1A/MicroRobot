#ifndef __TIMER_PC_H__
#define __TIMER_PC_H__

#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct TIMER_Base TIMER_Base_t;

typedef struct
{
    void (*TIMER_SET_NOTIFY_TASK)(TIMER_Base_t *self, TaskHandle_t task_handle);
    void (*TIMER_START)(TIMER_Base_t *self);
    void (*TIMER_STOP)(TIMER_Base_t *self);
} TIMER_Ops_t;

struct TIMER_Base
{
    const char *name;
    const TIMER_Ops_t *ops;
};

void TIMER_SET_NOTIFY_TASK(TIMER_Base_t *self, TaskHandle_t task_handle);
void TIMER_START(TIMER_Base_t *self);
void TIMER_STOP(TIMER_Base_t *self);

#endif

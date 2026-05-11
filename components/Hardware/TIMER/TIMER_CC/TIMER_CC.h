#ifndef __TIMER_CC_H__
#define __TIMER_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver/gptimer.h"
#include "TIMER_PC.h"

typedef struct
{
    TIMER_Base_t base;
    gptimer_handle_t timer;
    TaskHandle_t notify_task_handle;
    uint32_t period_us;
    bool is_started;
} ESPTIMER_Class_t;

void ESPTIMER_Init(ESPTIMER_Class_t *self, const char *name, uint32_t period_us);

#endif

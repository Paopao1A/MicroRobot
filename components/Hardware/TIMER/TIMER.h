#ifndef __TIMER_H__
#define __TIMER_H__

#include "TIMER_CC.h"
#include "TIMER_PC.h"

#define TIMER_PERIOD_MS 10
#define TIMER_PERIOD_US (TIMER_PERIOD_MS * 1000)

void TIMER_SetNotifyTask(TaskHandle_t task_handle);
void TIMER_Start(void);
void TIMER_Stop(void);

#endif

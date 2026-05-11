#include "TIMER.h"
#include "BOARD.h"

static ESPTIMER_Class_t s_legacy_timer;

static TIMER_Base_t *TIMER_Get_Default(void)
{
    if (Esp_TIMER == NULL)
    {
        ESPTIMER_Init(&s_legacy_timer, "esp_timer", TIMER_PERIOD_US);
        Esp_TIMER = &s_legacy_timer.base;
    }

    return Esp_TIMER;
}

void TIMER_SetNotifyTask(TaskHandle_t task_handle)
{
    TIMER_SET_NOTIFY_TASK(TIMER_Get_Default(), task_handle);
}

void TIMER_Start(void)
{
    TIMER_START(TIMER_Get_Default());
}

void TIMER_Stop(void)
{
    TIMER_STOP(TIMER_Get_Default());
}

#include "TIMER_PC.h"

void TIMER_SET_NOTIFY_TASK(TIMER_Base_t *self, TaskHandle_t task_handle)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->TIMER_SET_NOTIFY_TASK != NULL);
    self->ops->TIMER_SET_NOTIFY_TASK(self, task_handle);
}

void TIMER_START(TIMER_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->TIMER_START != NULL);
    self->ops->TIMER_START(self);
}

void TIMER_STOP(TIMER_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->TIMER_STOP != NULL);
    self->ops->TIMER_STOP(self);
}

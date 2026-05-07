#include "BEEP_PC.h"

void BEEP_ON(BEEP_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BEEP_ON != NULL);
    self->ops->BEEP_ON(self);
}

void BEEP_OFF(BEEP_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BEEP_OFF != NULL);
    self->ops->BEEP_OFF(self);
}

void BEEP_On(BEEP_Base_t *self)
{
    BEEP_ON(self);
}

void BEEP_Off(BEEP_Base_t *self)
{
    BEEP_OFF(self);
}

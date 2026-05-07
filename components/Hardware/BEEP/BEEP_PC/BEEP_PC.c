#include "BEEP_PC.h"

void BEEP_On(BEEP_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BEEP_On != NULL);
    self->ops->BEEP_On(self);
}

void BEEP_Off(BEEP_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->BEEP_Off != NULL);
    self->ops->BEEP_Off(self);
}


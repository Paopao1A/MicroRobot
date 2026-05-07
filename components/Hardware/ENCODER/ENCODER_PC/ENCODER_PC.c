#include "ENCODER_PC.h"

int ENCODER_GetCount(ENCODER_Base_t *self, encoder_id_t encoder_id)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ENCODER_GetCount != NULL);
    return self->ops->ENCODER_GetCount(self, encoder_id);
}


#include "ENCODER_PC.h"

int ENCODER_GET_COUNT(ENCODER_Base_t *self, encoder_id_t encoder_id)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ENCODER_GET_COUNT != NULL);
    return self->ops->ENCODER_GET_COUNT(self, encoder_id);
}

int ENCODER_GetCount(ENCODER_Base_t *self, encoder_id_t encoder_id)
{
    return ENCODER_GET_COUNT(self, encoder_id);
}

#include "ADC_PC.h"

void ADC_UPDATE(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_UPDATE != NULL);
    self->ops->ADC_UPDATE(self);
}

float ADC_GET_VOLTAGE(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_GET_VOLTAGE != NULL);
    return self->ops->ADC_GET_VOLTAGE(self);
}

uint16_t ADC_GET_RAW_DATA(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_GET_RAW_DATA != NULL);
    return self->ops->ADC_GET_RAW_DATA(self);
}


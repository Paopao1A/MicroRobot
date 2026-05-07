#include "ADC_PC.h"

void ADC_Update(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_Update != NULL);
    self->ops->ADC_Update(self);
}

float ADC_GetVoltage(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_GetVoltage != NULL);
    return self->ops->ADC_GetVoltage(self);
}

uint16_t ADC_GetRawData(ADC_Base_t *self)
{
    assert(self != NULL);
    assert(self->ops != NULL);
    assert(self->ops->ADC_GetRawData != NULL);
    return self->ops->ADC_GetRawData(self);
}


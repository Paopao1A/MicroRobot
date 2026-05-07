#ifndef __ADC_PC_H__
#define __ADC_PC_H__

#include <assert.h>
#include <stdint.h>

typedef struct ADC_Base ADC_Base_t;

typedef struct
{
    void (*ADC_Update)(ADC_Base_t *self);
    float (*ADC_GetVoltage)(ADC_Base_t *self);
    uint16_t (*ADC_GetRawData)(ADC_Base_t *self);
} ADC_Ops_t;

struct ADC_Base
{
    const char *name;
    const ADC_Ops_t *ops;
};

void ADC_Update(ADC_Base_t *self);
float ADC_GetVoltage(ADC_Base_t *self);
uint16_t ADC_GetRawData(ADC_Base_t *self);

#endif


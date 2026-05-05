#ifndef __LED_PC_H__
#define __LED_PC_H__

#include <assert.h>

typedef struct LED_Base LED_Base_t;

typedef struct
{
    void (*LED_On)(LED_Base_t *self);
    void (*LED_Off)(LED_Base_t *self);
    void (*LED_Flash)(LED_Base_t *self);
} LED_Ops_t;

struct LED_Base
{
    const char *name;
    const LED_Ops_t *ops;
};

void LED_On(LED_Base_t *self);
void LED_Off(LED_Base_t *self);
void LED_Flash(LED_Base_t *self);

#endif

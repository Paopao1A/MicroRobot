#ifndef __WIFI_PC_H__
#define __WIFI_PC_H__

#include <assert.h>
#include <stdbool.h>

typedef struct WIFI_Base WIFI_Base_t;

typedef struct
{
    void (*WIFI_CONNECT)(WIFI_Base_t *self);
    bool (*WIFI_IS_CONNECTED)(WIFI_Base_t *self);
} WIFI_Ops_t;

struct WIFI_Base
{
    const char *name;
    const WIFI_Ops_t *ops;
};

void WIFI_CONNECT(WIFI_Base_t *self);
bool WIFI_IS_CONNECTED(WIFI_Base_t *self);

#endif

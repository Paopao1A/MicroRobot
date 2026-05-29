#ifndef __LIDAR_PC_H__
#define __LIDAR_PC_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct LIDAR_Base LIDAR_Base_t;

#define LIDAR_POINT_MAX 360

typedef struct
{
    uint16_t Distance_Mm;
    uint8_t Intensity;
} LIDAR_Point_t;

typedef struct
{
    uint16_t Size;
    LIDAR_Point_t Points[LIDAR_POINT_MAX];
} LIDAR_Scan_t;

typedef struct
{
    void (*LIDAR_RECEIVE_BYTE)(LIDAR_Base_t *self, uint8_t data);
    bool (*LIDAR_HAS_NEW_SCAN)(LIDAR_Base_t *self);
    void (*LIDAR_CLEAR_NEW_SCAN)(LIDAR_Base_t *self);
    void (*LIDAR_GET_SCAN)(LIDAR_Base_t *self, LIDAR_Scan_t *scan);
} LIDAR_Ops_t;

struct LIDAR_Base
{
    const char *name;
    const LIDAR_Ops_t *ops;
};

void LIDAR_RECEIVE_BYTE(LIDAR_Base_t *self, uint8_t data);
bool LIDAR_HAS_NEW_SCAN(LIDAR_Base_t *self);
void LIDAR_CLEAR_NEW_SCAN(LIDAR_Base_t *self);
void LIDAR_GET_SCAN(LIDAR_Base_t *self, LIDAR_Scan_t *scan);

#endif

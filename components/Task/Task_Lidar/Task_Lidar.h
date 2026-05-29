#ifndef __TASK_LIDAR_H__
#define __TASK_LIDAR_H__

#include <stdbool.h>
#include <stdint.h>

#include "LIDAR.h"

#define TASK_LIDAR_SCAN_POINT_NUM    LIDAR_POINT_MAX
#define TASK_LIDAR_READ_BUFFER_SIZE  128

typedef struct
{
    LIDAR_Scan_t Scan;
    bool Is_Valid;
    uint32_t Tick_Ms;
    uint32_t Update_Count;
} Task_Lidar_Scan_t;

void Task_Lidar_Init(void);
void Task_Lidar_GetScan(Task_Lidar_Scan_t *Scan);

#endif

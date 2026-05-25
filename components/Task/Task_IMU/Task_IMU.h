#ifndef __TASK_IMU_H__
#define __TASK_IMU_H__

#include <stdbool.h>
#include <stdint.h>

#define TASK_IMU_PERIOD_MS 10

typedef struct
{
    int16_t Gyro_Raw[3];
    float Gyro_Radps[3];
    int16_t Accel_Raw[3];
    float Accel_Ms2[3];
    bool Is_Valid;
    uint32_t Update_Count;
    uint32_t Tick_Ms;
} Task_IMU_Snapshot_t;

void Task_IMU_Init(void);
void Task_IMU_GetSnapshot(Task_IMU_Snapshot_t *Snapshot);
float Task_IMU_GetAngularZRadps(void);

#endif

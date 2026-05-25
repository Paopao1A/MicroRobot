#ifndef __TASK_ODOMETRY_H__
#define __TASK_ODOMETRY_H__

#include <stdint.h>

#define TASK_ODOMETRY_PERIOD_MS        20
#define TASK_ODOMETRY_WHEEL_CIRCUM_M   0.1508f//轮胎半径
#define TASK_ODOMETRY_ROBOT_APB_M      0.115f

typedef struct
{
    float X_M;
    float Y_M;
    float Yaw_Rad;
    float Linear_X_Mps;
    float Linear_Y_Mps;
    float Angular_Z_Radps;
    uint32_t Update_Count;
} Task_Odometry_Data_t;

void Task_Odometry_Init(void);
void Task_Odometry_GetData(Task_Odometry_Data_t *Data);

#endif

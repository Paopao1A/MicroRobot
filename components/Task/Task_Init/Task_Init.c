#include "Task_Init.h"
#include "Task_BeepControl.h"
#include "Task_IMU.h"
#include "Task_Lidar.h"
#include "Task_MotionControl.h"
#include "Task_Odometry.h"
#include "Task_LedControl.h"
void Task_Init(void)
{
    Task_IMU_Init();
    Task_Lidar_Init();
    Task_MotionControl_Init();
    Task_Odometry_Init();
    Task_BeepControl_Init();
    Task_LedControl_Init();
}

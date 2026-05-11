#include "Task_Init.h"
#include "Task_BeepControl.h"
#include "Task_MotionControl.h"
void Task_Init(void)
{
    Task_MotionControl_Init();
    Task_BeepControl_Init();
}

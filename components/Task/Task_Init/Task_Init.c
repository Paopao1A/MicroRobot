#include "Task_Init.h"
#include "Task_BeepControl.h"
#include "Task_MotionControl.h"
#include "Task_LedControl.h"
void Task_Init(void)
{
    Task_MotionControl_Init();
    Task_BeepControl_Init();
    Task_LedControl_Init();
}

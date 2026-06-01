#ifndef __TASK_SERVO_CONTROL_H__
#define __TASK_SERVO_CONTROL_H__

typedef struct
{
    float Pan_Deg;
    float Tilt_Deg;
} Task_ServoControl_State_t;

void Task_ServoControl_Init(void);
void Task_ServoControl_GetState(Task_ServoControl_State_t *State);

#endif

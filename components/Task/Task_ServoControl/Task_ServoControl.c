#include "Task_ServoControl.h"

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "App_Subscriber_Servo.h"
#include "SERVO.h"
#include "SERVO_CC.h"

#define TASK_SERVO_CONTROL_STACK_SIZE    3072
#define TASK_SERVO_CONTROL_PRIORITY      5
#define TASK_SERVO_CONTROL_PERIOD_MS     20

static void Task_ServoControl(void *pvParameters);
static portMUX_TYPE s_Servo_State_Lock = portMUX_INITIALIZER_UNLOCKED;
static Task_ServoControl_State_t s_Servo_State = {
    .Pan_Deg = SERVO_PAN_DEFAULT_DEG,
    .Tilt_Deg = SERVO_TILT_DEFAULT_DEG,
};

static void Task_ServoControl_UpdateState(float Pan_Deg, float Tilt_Deg)
{
    portENTER_CRITICAL(&s_Servo_State_Lock);
    s_Servo_State.Pan_Deg = Pan_Deg;
    s_Servo_State.Tilt_Deg = Tilt_Deg;
    portEXIT_CRITICAL(&s_Servo_State_Lock);
}

void Task_ServoControl_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_ServoControl,
                "Task_ServoControl",
                TASK_SERVO_CONTROL_STACK_SIZE,
                NULL,
                TASK_SERVO_CONTROL_PRIORITY,
                &xTaskHandle);
}

void Task_ServoControl_GetState(Task_ServoControl_State_t *State)
{
    if (State == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_Servo_State_Lock);
    *State = s_Servo_State;
    portEXIT_CRITICAL(&s_Servo_State_Lock);
}

static void Task_ServoControl(void *pvParameters)
{
    float Last_Pan_Deg = SERVO_PAN_DEFAULT_DEG;
    float Last_Tilt_Deg = SERVO_TILT_DEFAULT_DEG;

    SERVO_Set_Angle(SERVO_ID_PAN, Last_Pan_Deg);
    SERVO_Set_Angle(SERVO_ID_TILT, Last_Tilt_Deg);
    Task_ServoControl_UpdateState(Last_Pan_Deg, Last_Tilt_Deg);

    while (1)
    {
        bool Is_Changed = false;
        App_Servo_Target_t Target;
        App_Subscriber_Servo_GetTarget(&Target);

        if (!Target.Is_Timeout)
        {
            if (Target.Pan_Deg != Last_Pan_Deg)
            {
                SERVO_Set_Angle(SERVO_ID_PAN, Target.Pan_Deg);
                Last_Pan_Deg = Target.Pan_Deg;
                Is_Changed = true;
            }

            if (Target.Tilt_Deg != Last_Tilt_Deg)
            {
                SERVO_Set_Angle(SERVO_ID_TILT, Target.Tilt_Deg);
                Last_Tilt_Deg = Target.Tilt_Deg;
                Is_Changed = true;
            }

            if (Is_Changed)
            {
                Task_ServoControl_UpdateState(Last_Pan_Deg, Last_Tilt_Deg);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_SERVO_CONTROL_PERIOD_MS));
    }
}

#include "Task_MotionControl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TIMER.h"

#define TASK_MOTION_CONTROL_STACK_SIZE    4096
#define TASK_MOTION_CONTROL_PRIORITY      5

static void Task_MotionControl(void *pvParameters);

void Task_MotionControl_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_MotionControl,
                "Task_MotionControl",
                TASK_MOTION_CONTROL_STACK_SIZE,
                NULL,
                TASK_MOTION_CONTROL_PRIORITY,
                &xTaskHandle);

    if (xTaskHandle != NULL)
    {
        TIMER_SetNotifyTask(xTaskHandle);//设置定时器通知任务句柄
    }
}

static void Task_MotionControl(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//等待定时器通知，10ms周期,PDTRUE表示接收到通知后将通知值清零，portMAX_DELAY表示无限等待。
    /*GPTimer 10ms 中断流程
    -> vTaskNotifyGiveFromISR(Task_MotionControl)
    -> Task_MotionControl 通知值 +1
    -> 如果任务正在 ulTaskNotifyTake() 等待，就唤醒它
    -> 如果它优先级更高，请求切换
    -> 中断退出
    -> Task_MotionControl 开始执行一次 10ms 控制逻辑
    */

    }

}

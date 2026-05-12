#include "Task_LedControl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BOARD.h"
#include "LED.h"


#define TASK_LED_CONTROL_STACK_SIZE    1024
#define TASK_LED_CONTROL_PRIORITY      3

static void Task_LedControl(void *pvParameters);

void Task_LedControl_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_LedControl,
                "Task_LedControl",
                TASK_LED_CONTROL_STACK_SIZE,
                NULL,
                TASK_LED_CONTROL_PRIORITY,
                &xTaskHandle);

    if(xTaskHandle == NULL)
    {
        printf("Task_LedControl_Init: xTaskCreate failed\n");
    }

}

void Task_LedControl(void *pvParameters)
{
    while(1)
    {
        LED_On(Esp_LED); 
        vTaskDelay(pdMS_TO_TICKS(500)); // 每隔500ms闪烁一次
        LED_Off(Esp_LED); 
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

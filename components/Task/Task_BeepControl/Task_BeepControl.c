#include "Task_BeepControl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ADC.h"
#include "BEEP.h"

#define TASK_BEEP_CONTROL_STACK_SIZE    2048
#define TASK_BEEP_CONTROL_PRIORITY      3

static void Task_BeepControl(void* pvParameters);

void Task_BeepControl_Init(void)
{
    TaskHandle_t *xTaskHandle = NULL;
    xTaskCreate(Task_BeepControl,
                "Task_BeepControl",
                TASK_BEEP_CONTROL_STACK_SIZE,
                NULL,
                TASK_BEEP_CONTROL_PRIORITY,
                xTaskHandle);
    if(xTaskHandle == NULL)
    {
        printf("Task_BeepControl_Init: xTaskCreate failed\n");
    }

}


static void Task_BeepControl(void* pvParameters)
{
    
    while(1)
    {
        ADC_UpdateVoltageData();
        if(Voltage_Data < 6.8f)
        {
            BEEP_on();
        }
        else
        {
            BEEP_off();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

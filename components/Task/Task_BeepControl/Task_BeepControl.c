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
    float sum_voltage = 0.0f;
    float average_voltage = 0.0f;
    while(1)
    {
        sum_voltage = 0.0f;
        for(uint8_t count = 0;count < 10;count++)
        {
            ADC_UpdateVoltageData();
            sum_voltage += Voltage_Data;//累加10次电压数据
        }
        average_voltage = sum_voltage / 10.0f;

        if(average_voltage < 7.0f && average_voltage > 6.0f)//防止检测到未开启电池的情况
        {
            BEEP_on();
        }
        else
        {
            BEEP_off();
        }
        printf("Average_Voltage: %f\n\r", average_voltage);
        vTaskDelay(pdMS_TO_TICKS(30000));//每30s检查一次电压
    }
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "BOARD.h"
#include "BEEP.h"
#include "ADC.h"
#include "ENCODER.h"
#include "IMU.h"
#include "TIMER.h"
#include "PID.h"
#include "BEEP.h"
#include "Task_Init.h"

void app_main(void)
{
    BOARD_Init(); // 初始化板级硬件
    BEEP_Init();
    ADC_Init();
    Task_Init();

    vTaskDelay(pdMS_TO_TICKS(1000)); // 等待系统稳定

    while(1)
    {
        LED_Flash(Esp_LED); // 通过LED父类接口控制LED闪烁
        vTaskDelay(pdMS_TO_TICKS(100)); // 延时100ms
    }
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"
#include "BEEP.h"
#include "ADC.h"
#include "MOTOR.h"
#include "PID.h"

void app_main(void)
{
    LED_Init();

    while(1)
    {
        LED_On();
        vTaskDelay(pdMS_TO_TICKS(500));
        LED_Off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

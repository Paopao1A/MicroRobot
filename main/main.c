#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "BOARD.h"
#include "IMU.h"
#include "TIMER.h"
#include "PID.h"
#include "Task_Init.h"
#include "App_Init.h"

void app_main(void)
{
    BOARD_Init();
    Task_Init();
    App_Init();

    BEEP_on();
    vTaskDelay(pdMS_TO_TICKS(1000));
    BEEP_off();

    while (1)
    {
        /* LED test */
        // LED_Flash(Esp_LED);

        /* IMU and I2C test */
        // float angular = IMU_Get_Angular();
        // printf("angular: %f\n", angular);

        /* Encoder test */
        // float speed = Encoder_Get_Speed(ENCODER_ID_M1);
        // int count = Encoder_Get_Count(ENCODER_ID_M1);
        // printf("encoder speed: %f, count: %d\n", speed, count);

        /* PWM test */
        // PWM_Set_Duty(PWM_ID_M1, 100);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

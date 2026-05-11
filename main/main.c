#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "BOARD.h"
#include "IMU.h"
#include "TIMER.h"
#include "PID.h"
#include "Task_Init.h"

void app_main(void)
{
    BOARD_Init(); // 初始化板级硬件
    Task_Init();

    vTaskDelay(pdMS_TO_TICKS(1000)); // 等待系统稳定

    while(1)
    {
        /*测试LED*/
        LED_Flash(Esp_LED); // 通过LED父类接口控制LED闪烁

        /*测试IMU和I2C是否正常*/
        //float angular = IMU_Get_Angular();
        //printf("angular: %f\n", angular);

        /*测试编码器*/
        //float speed = Encoder_Get_Speed(ENCODER_ID_M1);
        //int count = Encoder_Get_Count(ENCODER_ID_M1);
        //printf("encoder speed: %f, count: %d\n", speed, count);

        /*测试PWM*/
        //PWM_Set_Duty(PWM_ID_M1, 100);
        //vTaskDelay(pdMS_TO_TICKS(10)); // 延时1ms
    }
}

#include "LED.h"
#include "driver/gpio.h"

void LED_Init(void)
{
    gpio_config_t io_conf = {};
    // 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //设置为输出模式
    io_conf.mode = GPIO_MODE_OUTPUT;
    //引脚编号设置
    io_conf.pin_bit_mask = (1ULL<<LED_GPIO);
    //禁用下拉
    io_conf.pull_down_en = 0;
    //禁用上拉
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings 配置GPIO口
    gpio_config(&io_conf);
    // 关闭LED灯
    LED_Off();
}

void LED_On(void)
{
    gpio_set_level(LED_GPIO, 1);
}

void LED_Off(void)
{
    gpio_set_level(LED_GPIO, 0);
}

void LED_Flash(uint16_t interval)
{
    static uint16_t state = 0;
    static uint16_t count = 0;
    count++;
    if(count >= interval)
    {
        count = 0;
        state = !state;
        if(state)
        {
            LED_On();
        }
        else
        {
            LED_Off();
        }
    }
}

#include "BEEP.h"
#include "driver/gpio.h"

void BEEP_off(void);

void BEEP_Init(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode 设置为输出模式
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set 引脚编号设置
    io_conf.pin_bit_mask = (1ULL<<BEEP_GPIO);
    //disable pull-down mode 禁用下拉
    io_conf.pull_down_en = 0;
    //disable pull-up mode 禁用上拉
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings 配置GPIO口
    gpio_config(&io_conf);
    // 关闭蜂鸣器
    BEEP_off();
}

void BEEP_off(void)
{
    gpio_set_level(BEEP_GPIO, 0);
}


void BEEP_on(void)
{
    gpio_set_level(BEEP_GPIO, 1);
}


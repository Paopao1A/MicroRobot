#include "BOARD.h"
#include "LED.h"

LED_Base_t *Esp_LED;//定义LED父类对象，后续所有函数都操作父类

ESPLED_Class_t led;//定义LED子类对象


void BOARD_Init(void)//板级初始化函数
{
    LED_Init(&led,"esp_led",LED_GPIO,500);//初始化LED对象

    Esp_LED = &led.base;//将LED对象的父类地址赋值给Esp_LED，后续通过Esp_LED操作LED对象
}

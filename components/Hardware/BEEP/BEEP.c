#include "BEEP.h"
#include "BOARD.h"

static ESPBEEP_Class_t s_legacy_beep;//在板级初始化失效的时候，使用的备用蜂鸣器实例

static BEEP_Base_t *BEEP_Get_Default(void)//如果蜂鸣器未初始化，则初始化
{
    if (Esp_BEEP == NULL)
    {
        ESPBEEP_Init(&s_legacy_beep, "esp_beep", BEEP_GPIO);
        Esp_BEEP = &s_legacy_beep.base;
    }

    return Esp_BEEP;
}

void BEEP_on(void)//打开蜂鸣器
{
    BEEP_ON(BEEP_Get_Default());
}

void BEEP_off(void)//关闭蜂鸣器
{
    BEEP_OFF(BEEP_Get_Default());
}

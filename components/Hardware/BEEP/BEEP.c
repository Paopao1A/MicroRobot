#include "BEEP.h"
#include "BOARD.h"

static ESPBEEP_Class_t s_legacy_beep;

static BEEP_Base_t *BEEP_Get_Default(void)
{
    if (Esp_BEEP == NULL)
    {
        ESPBEEP_Init(&s_legacy_beep, "esp_beep", BEEP_GPIO);
        Esp_BEEP = &s_legacy_beep.base;
    }

    return Esp_BEEP;
}

void BEEP_Init(void)
{
    ESPBEEP_Init(&s_legacy_beep, "esp_beep", BEEP_GPIO);
    Esp_BEEP = &s_legacy_beep.base;
}

void BEEP_on(void)
{
    BEEP_On(BEEP_Get_Default());
}

void BEEP_off(void)
{
    BEEP_Off(BEEP_Get_Default());
}


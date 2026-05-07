#include "BEEP_CC.h"
#include "Hardware_Common.h"

static void ESPBEEP_On(BEEP_Base_t *self);
static void ESPBEEP_Off(BEEP_Base_t *self);

static const BEEP_Ops_t ESP_BEEP_Ops = {
    .BEEP_ON = ESPBEEP_On,
    .BEEP_OFF = ESPBEEP_Off,
};

static void ESPBEEP_On(BEEP_Base_t *self)
{
    ESPBEEP_Class_t *class = container_of(self, ESPBEEP_Class_t, base);
    gpio_set_level(class->gpio_num, 1);
}

static void ESPBEEP_Off(BEEP_Base_t *self)
{
    ESPBEEP_Class_t *class = container_of(self, ESPBEEP_Class_t, base);
    gpio_set_level(class->gpio_num, 0);
}

void ESPBEEP_Init(ESPBEEP_Class_t *self, const char *name, uint32_t gpio_num)
{
    self->base.name = name;
    self->base.ops = &ESP_BEEP_Ops;
    self->gpio_num = gpio_num;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << self->gpio_num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    ESPBEEP_Off(&self->base);
}

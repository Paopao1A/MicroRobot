#include "LED_CC.h"
#include "Hardware_Common.h"

static void ESPLED_On(LED_Base_t *self);
static void ESPLED_Off(LED_Base_t *self);
static void ESPLED_Flash(LED_Base_t *self);

static const LED_Ops_t ESP_LED_Ops = {
    .LED_ON = ESPLED_On,
    .LED_OFF = ESPLED_Off,
    .LED_FLASH = ESPLED_Flash,
};

static void ESPLED_On(LED_Base_t *self)
{
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);
    gpio_set_level(class->gpio_num, 1);
    class->flash_state = true;
}

static void ESPLED_Off(LED_Base_t *self)
{
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);
    gpio_set_level(class->gpio_num, 0);
    class->flash_state = false;
}

static void ESPLED_Flash(LED_Base_t *self)
{
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);

    class->flash_count++;
    if (class->flash_count < class->interval)
    {
        return;
    }

    class->flash_count = 0;
    if (class->flash_state)
    {
        ESPLED_Off(self);
    }
    else
    {
        ESPLED_On(self);
    }
}

void ESPLED_Init(ESPLED_Class_t *self, const char *name, gpio_num_t gpio_num, uint16_t interval)
{
    self->base.name = name;
    self->base.ops = &ESP_LED_Ops;
    self->gpio_num = gpio_num;
    self->interval = (interval == 0) ? 1 : interval;
    self->flash_count = 0;
    self->flash_state = false;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << self->gpio_num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    ESPLED_Off(&self->base);
}

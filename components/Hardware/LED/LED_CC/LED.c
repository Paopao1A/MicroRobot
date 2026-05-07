#include "LED.h"
#include "BOARD.h"

static void ESPLED_On(LED_Base_t *self);
static void ESPLED_Off(LED_Base_t *self);
static void ESPLED_Flash(LED_Base_t *self);

static const LED_Ops_t ESP_LED_Ops = {
    .LED_On = ESPLED_On,
    .LED_Off = ESPLED_Off,
    .LED_Flash = ESPLED_Flash,
};



static void ESPLED_On(LED_Base_t *self)
{
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);
    gpio_set_level(class->gpio_num, 1);
}

static void ESPLED_Off(LED_Base_t *self)
{
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);
    gpio_set_level(class->gpio_num, 0);
}

static void ESPLED_Flash(LED_Base_t *self)
{
    static uint16_t state = 0;
    static uint16_t count = 0;
    ESPLED_Class_t *class = container_of(self, ESPLED_Class_t, base);

    count++;
    if (count >= class->interval)
    {
        count = 0;
        state = !state;
        if (state)
        {
            ESPLED_On(self);
        }
        else
        {
            ESPLED_Off(self);
        }
    }
}

void LED_Init(ESPLED_Class_t *self,char *name,uint32_t gpio_num,uint16_t interval)
{
    self->base.name = name;
    self->base.ops = &ESP_LED_Ops;
    self->gpio_num = gpio_num;
    self->interval = interval;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << self->gpio_num);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    ESPLED_Off(&self->base);
}

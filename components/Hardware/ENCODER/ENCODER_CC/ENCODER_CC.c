#include "ENCODER_CC.h"

#include "esp_err.h"
#include "Hardware_Common.h"

static int ESPENCODER_GetCount(ENCODER_Base_t *self, encoder_id_t encoder_id);

static const ENCODER_Ops_t ESP_ENCODER_Ops = {
    .ENCODER_GET_COUNT = ESPENCODER_GetCount,
};

static void ESPENCODER_InitOne(ESPENCODER_Class_t *class,
                               int index,
                               int edge_gpio_a,
                               int level_gpio_a,
                               int edge_gpio_b,
                               int level_gpio_b)
{
    pcnt_unit_config_t unit_config = {
        .high_limit = ENCODER_PCNT_HIGH_LIMIT,
        .low_limit = ENCODER_PCNT_LOW_LIMIT,
        .flags.accum_count = true,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = edge_gpio_a,
        .level_gpio_num = level_gpio_a,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = edge_gpio_b,
        .level_gpio_num = level_gpio_b,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, ENCODER_PCNT_HIGH_LIMIT));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, ENCODER_PCNT_LOW_LIMIT));
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    class->units[index] = pcnt_unit;
}

static int ESPENCODER_GetCount(ENCODER_Base_t *self, encoder_id_t encoder_id)
{
    ESPENCODER_Class_t *class = container_of(self, ESPENCODER_Class_t, base);
    int index = (int)encoder_id - 1;
    int count = 0;

    if (index < 0 || index >= ENCODER_NUM || class->units[index] == NULL)
    {
        return 0;
    }

    pcnt_unit_get_count(class->units[index], &count);
    return count;
}

void ESPENCODER_Init(ESPENCODER_Class_t *self, const char *name)
{
    self->base.name = name;
    self->base.ops = &ESP_ENCODER_Ops;

    ESPENCODER_InitOne(self, 0, ENCODER_GPIO_H1A, ENCODER_GPIO_H1B, ENCODER_GPIO_H1B, ENCODER_GPIO_H1A);
    ESPENCODER_InitOne(self, 1, ENCODER_GPIO_H2A, ENCODER_GPIO_H2B, ENCODER_GPIO_H2B, ENCODER_GPIO_H2A);
    ESPENCODER_InitOne(self, 2, ENCODER_GPIO_H3B, ENCODER_GPIO_H3A, ENCODER_GPIO_H3A, ENCODER_GPIO_H3B);
    ESPENCODER_InitOne(self, 3, ENCODER_GPIO_H4B, ENCODER_GPIO_H4A, ENCODER_GPIO_H4A, ENCODER_GPIO_H4B);
}

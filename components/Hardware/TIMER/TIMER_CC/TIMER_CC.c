#include "TIMER_CC.h"

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "Hardware_Common.h"

static const char *TAG = "TIMER";

static void ESPTIMER_SetNotifyTask(TIMER_Base_t *self, TaskHandle_t task_handle);
static void ESPTIMER_Start(TIMER_Base_t *self);
static void ESPTIMER_Stop(TIMER_Base_t *self);

static const TIMER_Ops_t ESP_TIMER_Ops = {
    .TIMER_SET_NOTIFY_TASK = ESPTIMER_SetNotifyTask,
    .TIMER_START = ESPTIMER_Start,
    .TIMER_STOP = ESPTIMER_Stop,
};

static bool IRAM_ATTR ESPTIMER_AlarmCallback(gptimer_handle_t timer,
                                             const gptimer_alarm_event_data_t *edata,
                                             void *user_ctx)
{
    ESPTIMER_Class_t *class = (ESPTIMER_Class_t *)user_ctx;
    BaseType_t high_task_woken = pdFALSE;

    (void)timer;//时间器句柄参数未使用
    (void)edata;//报警事件数据参数未使用

    if (class->notify_task_handle != NULL)
    {//发送任务通知，通知被发送后，等待该通知的任务将被唤醒
        vTaskNotifyGiveFromISR(class->notify_task_handle, &high_task_woken);
    }

    return high_task_woken == pdTRUE;
}

//设置定时器通知任务句柄，在相应任务函数进行填充
static void ESPTIMER_SetNotifyTask(TIMER_Base_t *self, TaskHandle_t task_handle)
{
    ESPTIMER_Class_t *class = container_of(self, ESPTIMER_Class_t, base);
    class->notify_task_handle = task_handle;
}

static void ESPTIMER_Start(TIMER_Base_t *self)
{
    ESPTIMER_Class_t *class = container_of(self, ESPTIMER_Class_t, base);

    if (class->is_started)
    {
        return;
    }

    ESP_ERROR_CHECK(gptimer_start(class->timer));
    class->is_started = true;
}

static void ESPTIMER_Stop(TIMER_Base_t *self)
{
    ESPTIMER_Class_t *class = container_of(self, ESPTIMER_Class_t, base);

    if (!class->is_started)
    {
        return;
    }

    ESP_ERROR_CHECK(gptimer_stop(class->timer));
    class->is_started = false;
}

void ESPTIMER_Init(ESPTIMER_Class_t *self, const char *name, uint32_t period_us)
{
    self->base.name = name;
    self->base.ops = &ESP_TIMER_Ops;
    self->notify_task_handle = NULL;
    self->period_us = (period_us == 0) ? 1 : period_us;
    self->is_started = false;

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &self->timer));

    gptimer_event_callbacks_t callbacks = {
        .on_alarm = ESPTIMER_AlarmCallback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(self->timer, &callbacks, self));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = self->period_us,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(self->timer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(self->timer));

    ESP_LOGI(TAG, "Init %luus hardware timer", (unsigned long)self->period_us);
}

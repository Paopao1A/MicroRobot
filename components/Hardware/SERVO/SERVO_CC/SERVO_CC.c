#include "SERVO_CC.h"

#include "esp_err.h"
#include "esp_log.h"
#include "Hardware_Common.h"

static const char *TAG = "SERVO";

static void ESPSERVO_SetAngle(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg);
static void ESPSERVO_SetPulseUs(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us);

static const SERVO_Ops_t ESP_SERVO_Ops = {
    .SERVO_SET_ANGLE = ESPSERVO_SetAngle,
    .SERVO_SET_PULSE_US = ESPSERVO_SetPulseUs,
};

static int ESPSERVO_GetIndex(servo_id_t servo_id)
{
    int index = (int)servo_id - 1;
    if (index < 0 || index >= SERVO_NUM)
    {
        return -1;
    }
    return index;
}

//限制舵机角度在有效范围内
static float ESPSERVO_LimitAngle(float angle_deg)
{
    if (angle_deg > SERVO_ANGLE_MAX_DEG)
    {
        return SERVO_ANGLE_MAX_DEG;
    }
    if (angle_deg < SERVO_ANGLE_MIN_DEG)
    {
        return SERVO_ANGLE_MIN_DEG;
    }
    return angle_deg;
}

static float ESPSERVO_LimitAngleById(servo_id_t servo_id, float angle_deg)
{
    float min_deg = SERVO_ANGLE_MIN_DEG;
    float max_deg = SERVO_ANGLE_MAX_DEG;

    if (servo_id == SERVO_ID_PAN)//如果是底部舵机，限制角度在0-180度
    {
        min_deg = SERVO_PAN_MIN_DEG;
        max_deg = SERVO_PAN_MAX_DEG;
    }
    else if (servo_id == SERVO_ID_TILT)//如果是顶部舵机，限制角度在0-110度
    {
        min_deg = SERVO_TILT_MIN_DEG;
        max_deg = SERVO_TILT_MAX_DEG;
    }

    if (angle_deg > max_deg)
    {
        return max_deg;
    }
    if (angle_deg < min_deg)
    {
        return min_deg;
    }
    return angle_deg;
}

//限制舵机脉冲宽度在有效范围内
static uint32_t ESPSERVO_LimitPulseUs(uint32_t pulse_us)
{
    if (pulse_us > SERVO_PULSE_MAX_US)
    {
        return SERVO_PULSE_MAX_US;
    }
    if (pulse_us < SERVO_PULSE_MIN_US)
    {
        return SERVO_PULSE_MIN_US;
    }
    return pulse_us;
}

static uint32_t ESPSERVO_AngleToPulseUs(float angle_deg)
{
    angle_deg = ESPSERVO_LimitAngle(angle_deg);
    float ratio = (angle_deg - SERVO_ANGLE_MIN_DEG) / (SERVO_ANGLE_MAX_DEG - SERVO_ANGLE_MIN_DEG);
    return (uint32_t)((float)SERVO_PULSE_MIN_US +
                      ratio * (float)(SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US));
}

static uint32_t ESPSERVO_PulseUsToDuty(uint32_t pulse_us)
{
    uint32_t period_us = 1000000UL / SERVO_FREQ_HZ;
    uint32_t duty_max = (1UL << SERVO_TIMER_RESOLUTION) - 1UL;
    return (pulse_us * duty_max) / period_us;//pulse_us/period_us就是实际占空比，然后乘分辨率就是最终填入
}

static void ESPSERVO_SetPulseOne(ESPSERVO_Class_t *class, int index, uint32_t pulse_us)
{
    pulse_us = ESPSERVO_LimitPulseUs(pulse_us);
    uint32_t duty = ESPSERVO_PulseUsToDuty(pulse_us);
    ESP_ERROR_CHECK(ledc_set_duty(SERVO_MODE, class->servos[index].channel, duty));//设置占空比
    ESP_ERROR_CHECK(ledc_update_duty(SERVO_MODE, class->servos[index].channel));//更新占空比，固定步骤
}

static void ESPSERVO_SetPulseUs(SERVO_Base_t *self, servo_id_t servo_id, uint32_t pulse_us)
{
    ESPSERVO_Class_t *class = container_of(self, ESPSERVO_Class_t, base);

    if (servo_id == SERVO_ID_ALL)
    {
        for (int i = 0; i < SERVO_NUM; i++)
        {
            ESPSERVO_SetPulseOne(class, i, pulse_us);
        }
        return;
    }

    int index = ESPSERVO_GetIndex(servo_id);
    if (index >= 0)
    {
        ESPSERVO_SetPulseOne(class, index, pulse_us);
    }
}

static void ESPSERVO_SetAngle(SERVO_Base_t *self, servo_id_t servo_id, float angle_deg)
{
    if (servo_id == SERVO_ID_ALL)
    {
        ESPSERVO_SetAngle(self, SERVO_ID_PAN, angle_deg);
        ESPSERVO_SetAngle(self, SERVO_ID_TILT, angle_deg);
        return;
    }

    angle_deg = ESPSERVO_LimitAngleById(servo_id, angle_deg);
    ESPSERVO_SetPulseUs(self, servo_id, ESPSERVO_AngleToPulseUs(angle_deg));
}

static void ESPSERVO_InitOne(ESPSERVO_Class_t *self,
                             int index,
                             gpio_num_t gpio_num,
                             ledc_channel_t channel)
{
    self->servos[index].gpio_num = gpio_num;
    self->servos[index].channel = channel;

    ledc_channel_config_t channel_config = {
        .gpio_num = gpio_num,
        .speed_mode = SERVO_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = SERVO_TIMER,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
}

//由于电机把PWM资源占用的差不多了，就用LEDC来控制舵机，LEDC同样可以生成PWM信号
void ESPSERVO_Init(ESPSERVO_Class_t *self, const char *name)
{
    self->base.name = name;
    self->base.ops = &ESP_SERVO_Ops;

    ESP_LOGI(TAG, "Init servo device");

    ledc_timer_config_t timer_config = {
        .speed_mode = SERVO_MODE,//速度模式
        .duty_resolution = SERVO_TIMER_RESOLUTION,//占空比分辨率
        .timer_num = SERVO_TIMER,//选择LEDC定时器0
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,//时钟源
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ESPSERVO_InitOne(self, 0, SERVO_GPIO_PAN, LEDC_CHANNEL_0);//初始化底部舵机
    ESPSERVO_InitOne(self, 1, SERVO_GPIO_TILT, LEDC_CHANNEL_1);//初始化顶部舵机
    ESPSERVO_SetAngle(&self->base, SERVO_ID_PAN, SERVO_PAN_DEFAULT_DEG);
    ESPSERVO_SetAngle(&self->base, SERVO_ID_TILT, SERVO_TILT_DEFAULT_DEG);//设置所有舵机角度到中心位置，默认位置
}

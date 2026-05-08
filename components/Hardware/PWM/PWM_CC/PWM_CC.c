#include "PWM_CC.h"

#include "esp_err.h"
#include "esp_log.h"
#include "Hardware_Common.h"

static const char *TAG = "PWM";

static void ESPPWM_Set(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle);
static void ESPPWM_Stop(PWM_Base_t *self, pwm_id_t pwm_id, bool brake);

static const PWM_Ops_t ESP_PWM_Ops = {
    .PWM_SET = ESPPWM_Set,
    .PWM_STOP = ESPPWM_Stop,
};

static int ESPPWM_GetIndex(pwm_id_t pwm_id)//获取电机索引
{
    int index = (int)pwm_id - 1;
    if (index < 0 || index >= PWM_MOTOR_NUM)
    {
        return -1;
    }
    return index;
}

static int ESPPWM_IgnoreDeadZone(int duty_cycle)//忽略死区，保证电机正常启动
{
    if (duty_cycle > 0)
    {
        return duty_cycle + PWM_MOTOR_DEAD_ZONE;
    }
    if (duty_cycle < 0)
    {
        return duty_cycle - PWM_MOTOR_DEAD_ZONE;
    }
    return 0;
}

static int ESPPWM_LimitDuty(int duty_cycle)//输出限制，防止超过定时器周期导致无法输出PWM信号
{
    if (duty_cycle > PWM_MOTOR_DUTY_TICK_MAX)
    {
        return PWM_MOTOR_DUTY_TICK_MAX;
    }
    if (duty_cycle < -PWM_MOTOR_DUTY_TICK_MAX)
    {
        return -PWM_MOTOR_DUTY_TICK_MAX;
    }
    return duty_cycle;
}

static void ESPPWM_EnableGenerator(mcpwm_gen_handle_t generator)//使能输出，输出PWM信号
{
    ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generator, -1, true));
}

static void ESPPWM_ForceGenerator(mcpwm_gen_handle_t generator, int level)
{
    //level=0强制低电平，level=1强制高电平，level=-1正常输出
    ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generator, level, true));
}

static void ESPPWM_SetOne(ESPPWM_Class_t *class, int index, int duty_cycle)
{
    ESPPWM_Motor_t *motor = &class->motors[index];

    duty_cycle = ESPPWM_IgnoreDeadZone(duty_cycle);
    duty_cycle = ESPPWM_LimitDuty(duty_cycle);

    if (duty_cycle > 0)//正转
    {
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(motor->comparator_a, duty_cycle));
        ESPPWM_EnableGenerator(motor->generator_a);//A路输出PWM
        ESPPWM_ForceGenerator(motor->generator_b, 0);//B路直接拉低
    }
    else if (duty_cycle < 0)//反转
    {
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(motor->comparator_b, -duty_cycle));
        ESPPWM_ForceGenerator(motor->generator_a, 0);//A路直接拉低
        ESPPWM_EnableGenerator(motor->generator_b); //B路输出PWM
    }
    else
    {
        ESPPWM_ForceGenerator(motor->generator_a, 0);
        ESPPWM_ForceGenerator(motor->generator_b, 0);
    }
}

static void ESPPWM_StopOne(ESPPWM_Motor_t *motor, bool brake)
{
    if (brake)
    {
        ESPPWM_ForceGenerator(motor->generator_a, 1);
        ESPPWM_ForceGenerator(motor->generator_b, 1);
    }
    else
    {
        ESPPWM_ForceGenerator(motor->generator_a, 0);
        ESPPWM_ForceGenerator(motor->generator_b, 0);
    }
}

static void ESPPWM_Set(PWM_Base_t *self, pwm_id_t pwm_id, int duty_cycle)
{
    ESPPWM_Class_t *class = container_of(self, ESPPWM_Class_t, base);

    if (pwm_id == PWM_ID_ALL)
    {
        for (int i = 0; i < PWM_MOTOR_NUM; i++)
        {
            ESPPWM_SetOne(class, i, duty_cycle);
        }
        return;
    }

    int index = ESPPWM_GetIndex(pwm_id);
    if (index >= 0)
    {
        ESPPWM_SetOne(class, index, duty_cycle);
    }
}

static void ESPPWM_Stop(PWM_Base_t *self, pwm_id_t pwm_id, bool brake)
{
    ESPPWM_Class_t *class = container_of(self, ESPPWM_Class_t, base);

    if (pwm_id == PWM_ID_ALL)
    {
        for (int i = 0; i < PWM_MOTOR_NUM; i++)
        {
            ESPPWM_StopOne(&class->motors[i], brake);
        }
        return;
    }

    int index = ESPPWM_GetIndex(pwm_id);
    if (index >= 0)
    {
        ESPPWM_StopOne(&class->motors[index], brake);
    }
}

static void ESPPWM_ConfigGenerator(mcpwm_gen_handle_t generator,
                                   mcpwm_cmpr_handle_t comparator)
{
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                     MCPWM_TIMER_EVENT_EMPTY,
                                     MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                       comparator,
                                       MCPWM_GEN_ACTION_LOW)));
    ESPPWM_ForceGenerator(generator, 0);
}

static void ESPPWM_InitOne(ESPPWM_Class_t *class,
                           int index,
                           int pwma_gpio_num,
                           int pwmb_gpio_num,
                           int group_id)
{
    ESPPWM_Motor_t *motor = &class->motors[index];

    mcpwm_timer_config_t timer_config = {
        .group_id = group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = PWM_MOTOR_TIMER_RESOLUTION_HZ,
        .period_ticks = PWM_MOTOR_DUTY_TICK_MAX,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &motor->timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = group_id,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &motor->oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(motor->oper, motor->timer));

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(motor->oper, &comparator_config, &motor->comparator_a));
    ESP_ERROR_CHECK(mcpwm_new_comparator(motor->oper, &comparator_config, &motor->comparator_b));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(motor->comparator_a, 0));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(motor->comparator_b, 0));

    mcpwm_generator_config_t generator_a_config = {
        .gen_gpio_num = pwma_gpio_num,
    };
    mcpwm_generator_config_t generator_b_config = {
        .gen_gpio_num = pwmb_gpio_num,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(motor->oper, &generator_a_config, &motor->generator_a));
    ESP_ERROR_CHECK(mcpwm_new_generator(motor->oper, &generator_b_config, &motor->generator_b));

    ESPPWM_ConfigGenerator(motor->generator_a, motor->comparator_a);
    ESPPWM_ConfigGenerator(motor->generator_b, motor->comparator_b);

    ESP_ERROR_CHECK(mcpwm_timer_enable(motor->timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(motor->timer, MCPWM_TIMER_START_NO_STOP));
}

void ESPPWM_Init(ESPPWM_Class_t *self, const char *name)
{
    self->base.name = name;
    self->base.ops = &ESP_PWM_Ops;

    ESP_LOGI(TAG, "Init PWM motor device");
    ESPPWM_InitOne(self, 0, PWM_GPIO_M1B, PWM_GPIO_M1A, PWM_MOTOR_TIMER_GROUP_ID_M1);
    ESPPWM_InitOne(self, 1, PWM_GPIO_M2B, PWM_GPIO_M2A, PWM_MOTOR_TIMER_GROUP_ID_M2);
    ESPPWM_InitOne(self, 2, PWM_GPIO_M3A, PWM_GPIO_M3B, PWM_MOTOR_TIMER_GROUP_ID_M3);
    ESPPWM_InitOne(self, 3, PWM_GPIO_M4A, PWM_GPIO_M4B, PWM_MOTOR_TIMER_GROUP_ID_M4);
}

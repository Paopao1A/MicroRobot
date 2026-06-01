#include "App_Subscriber_Servo.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "geometry_msgs/msg/vector3.h"
#include "rclc/subscription.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "SERVO_CC.h"

#if APP_SERVO_CMD_LOG_ENABLE
static const char *TAG = "App_Subscriber_Servo";
#endif

static rcl_subscription_t s_servo_subscriber;
static geometry_msgs__msg__Vector3 s_servo_msg;
static portMUX_TYPE s_Servo_Lock = portMUX_INITIALIZER_UNLOCKED;
static App_Servo_Target_t s_Servo_Target = {
    .Pan_Deg = SERVO_PAN_DEFAULT_DEG,
    .Tilt_Deg = SERVO_TILT_DEFAULT_DEG,
    .Is_Timeout = false,
};
static TickType_t s_Servo_LastTick = 0;

static float App_Servo_LimitFloat(float Value, float Min, float Max)
{
    if (Value > Max)
    {
        return Max;
    }
    if (Value < Min)
    {
        return Min;
    }
    return Value;
}

void App_Subscriber_Servo_GetTarget(App_Servo_Target_t *Target)
{
    if (Target == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_Servo_Lock);
    *Target = s_Servo_Target;
    TickType_t LastTick = s_Servo_LastTick;
    portEXIT_CRITICAL(&s_Servo_Lock);

    if ((LastTick == 0) ||
        ((xTaskGetTickCount() - LastTick) > pdMS_TO_TICKS(APP_SERVO_CMD_TIMEOUT_MS)))
    {
        Target->Is_Timeout = true;
    }
    else
    {
        Target->Is_Timeout = false;
    }
}

static void App_ServoCallback(const void *msgin)
{
    const geometry_msgs__msg__Vector3 *Msg = (const geometry_msgs__msg__Vector3 *)msgin;

    float Pan_Deg = App_Servo_LimitFloat((float)Msg->x,
                                         SERVO_PAN_MIN_DEG,
                                         SERVO_PAN_MAX_DEG);
    float Tilt_Deg = App_Servo_LimitFloat((float)Msg->y,
                                          SERVO_TILT_MIN_DEG,
                                          SERVO_TILT_MAX_DEG);

    portENTER_CRITICAL(&s_Servo_Lock);
    s_Servo_Target.Pan_Deg = Pan_Deg;
    s_Servo_Target.Tilt_Deg = Tilt_Deg;
    s_Servo_LastTick = xTaskGetTickCount();
    portEXIT_CRITICAL(&s_Servo_Lock);

#if APP_SERVO_CMD_LOG_ENABLE
    ESP_LOGI(TAG, "servo_cmd pan: %.2f deg, tilt: %.2f deg", Pan_Deg, Tilt_Deg);
#endif
}

rcl_ret_t App_Subscriber_Servo_Init(rcl_node_t *node, rclc_executor_t *executor)
{
    rcl_ret_t ret = rclc_subscription_init_default(&s_servo_subscriber,
                                                   node,
                                                   ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Vector3),
                                                   APP_SERVO_CMD_TOPIC);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    return rclc_executor_add_subscription(executor,
                                          &s_servo_subscriber,
                                          &s_servo_msg,
                                          App_ServoCallback,
                                          ON_NEW_DATA);
}

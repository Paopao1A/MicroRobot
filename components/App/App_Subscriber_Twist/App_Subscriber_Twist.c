#include "App_Subscriber_Twist.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "geometry_msgs/msg/twist.h"
#include "PID.h"
#include "rclc/subscription.h"
#include "rosidl_runtime_c/message_type_support_struct.h"

#if APP_CMD_VEL_LOG_ENABLE
static const char *TAG = "App_Subscriber_Twist";
#endif

static rcl_subscription_t s_twist_subscriber;//速度角速度订阅
static geometry_msgs__msg__Twist s_twist_msg;//速度角速度消息
static portMUX_TYPE s_CmdVel_Lock = portMUX_INITIALIZER_UNLOCKED;//速度角速度目标锁
static App_CmdVel_Target_t s_CmdVel_Target = {
    .Speed_RPM = 0.0f,
    .Angular_Radps = 0.0f,
    .Is_Timeout = false,
};
static TickType_t s_CmdVel_LastTick = 0;//速度角速度目标最后更新时间

static float App_LimitFloat(float Value, float Min, float Max)
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

//获取速度角速度目标
void App_Subscriber_Twist_GetCmdVelTarget(App_CmdVel_Target_t *Target)
{
    if (Target == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_CmdVel_Lock);
    *Target = s_CmdVel_Target;
    TickType_t LastTick = s_CmdVel_LastTick;
    portEXIT_CRITICAL(&s_CmdVel_Lock);

    if ((LastTick == 0) ||
        ((xTaskGetTickCount() - LastTick) > pdMS_TO_TICKS(APP_CMD_VEL_TIMEOUT_MS)))//超时
    {
        Target->Speed_RPM = 0.0f;
        Target->Angular_Radps = 0.0f;
        Target->Is_Timeout = true;
    }
    else
    {
        Target->Is_Timeout = false;
    }
}

//线速度转换为转速
static float App_LinearMpsToRpm(float Linear_Mps)
{
    return Linear_Mps * 60.0f / APP_WHEEL_CIRCUMFERENCE_M;
}

static void App_TwistCallback(const void *msgin)
{
    const geometry_msgs__msg__Twist *Msg = (const geometry_msgs__msg__Twist *)msgin;

    //获取线速度
    float Linear_Mps = (float)Msg->linear.x;

    //获取转速
    float Speed_Target_RPM = App_LimitFloat(App_LinearMpsToRpm(Linear_Mps),
                                            -APP_SPEED_TARGET_MAX_RPM,
                                            APP_SPEED_TARGET_MAX_RPM);
    float Angular_Radps = App_LimitFloat((float)Msg->angular.z,
                                         -APP_ANGULAR_TARGET_MAX_RADPS,
                                         APP_ANGULAR_TARGET_MAX_RADPS);

    //更新速度角速度目标，在临界区保护，防止多任务程同时更新
    portENTER_CRITICAL(&s_CmdVel_Lock);
    s_CmdVel_Target.Speed_RPM = Speed_Target_RPM;
    s_CmdVel_Target.Angular_Radps = Angular_Radps;
    s_CmdVel_LastTick = xTaskGetTickCount();//更新时间
    portEXIT_CRITICAL(&s_CmdVel_Lock);

#if APP_CMD_VEL_LOG_ENABLE
    ESP_LOGI(TAG,
             "cmd_vel linear: %.3f m/s -> %.3f rpm, angular: %.3f rad/s",
             Linear_Mps,
             Speed_Target_RPM,
             Angular_Radps);
#endif
}

rcl_ret_t App_Subscriber_Twist_Init(rcl_node_t *node, rclc_executor_t *executor)
{
    rcl_ret_t ret = rclc_subscription_init_default(&s_twist_subscriber,
                                                   node,
                                                   ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
                                                   APP_CMD_VEL_TOPIC);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    return rclc_executor_add_subscription(executor,
                                          &s_twist_subscriber,
                                          &s_twist_msg,
                                          App_TwistCallback,
                                          ON_NEW_DATA);
}

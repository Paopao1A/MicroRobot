#include "App_Subscriber.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "geometry_msgs/msg/twist.h"
#include "PID.h"
#include "rclc/subscription.h"
#include "rosidl_runtime_c/message_type_support_struct.h"

static const char *TAG = "App_Subscriber";

static rcl_subscription_t s_twist_subscriber;//订阅速度和角速度目标
static geometry_msgs__msg__Twist s_twist_msg;//速度和角速度目标消息
static portMUX_TYPE s_CmdVel_Lock = portMUX_INITIALIZER_UNLOCKED;//临界区锁
static App_CmdVel_Target_t s_CmdVel_Target = {
    .Speed_RPM = 0.0f,
    .Angular_Radps = 0.0f,
    .Is_Timeout = false,
};
static TickType_t s_CmdVel_LastTick = 0;

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

void App_Subscriber_GetCmdVelTarget(App_CmdVel_Target_t *Target)
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
        ((xTaskGetTickCount() - LastTick) > pdMS_TO_TICKS(APP_CMD_VEL_TIMEOUT_MS)))//如果上次接收时间是0，或者当前时间超过超时时间10s，认为是超时
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

//将线速度从m/s转换为转/分钟，用于实际PID控制，PID目标值单位是转/分钟
static float App_LinearMpsToRpm(float Linear_Mps)
{
    return Linear_Mps * 60.0f / APP_WHEEL_CIRCUMFERENCE_M;
}

static void App_TwistCallback(const void *msgin)
{
    const geometry_msgs__msg__Twist *Msg = (const geometry_msgs__msg__Twist *)msgin;

    //线速度信息，单位是m/s，1 m/s -> 约 398 r/min，所以上位机线速度输入值要小一些，比如0.1 m/s -> 约 39.8 r/min
    float Linear_Mps = (float)Msg->linear.x;

    //限制线速度和角速度的范围
    float Speed_Target_RPM = App_LimitFloat(App_LinearMpsToRpm(Linear_Mps),
                                            -APP_SPEED_TARGET_MAX_RPM,
                                            APP_SPEED_TARGET_MAX_RPM);
    float Angular_Radps = App_LimitFloat((float)Msg->angular.z,
                                         -APP_ANGULAR_TARGET_MAX_RADPS,
                                         APP_ANGULAR_TARGET_MAX_RADPS);

    //创建临界区，在临界区操作，数据很少用临界区合适，用互斥锁可能导致阻塞影响实时性，直接用临界区操作
    portENTER_CRITICAL(&s_CmdVel_Lock);
    s_CmdVel_Target.Speed_RPM = Speed_Target_RPM;//用中间变量存储，好处是和任务层解耦，APP层不依赖任务层的PID控制。更清晰，控制节拍更稳定，后续扩展更舒服
    s_CmdVel_Target.Angular_Radps = Angular_Radps;
    s_CmdVel_LastTick = xTaskGetTickCount();//更新时间，如果上位机迟迟没有发送信息，认为是超时
    portEXIT_CRITICAL(&s_CmdVel_Lock);

    ESP_LOGI(TAG,
             "cmd_vel linear: %.3f m/s -> %.3f rpm, angular: %.3f rad/s",
             Linear_Mps,
             Speed_Target_RPM,
             Angular_Radps);
}

rcl_ret_t App_Subscriber_Init(rcl_node_t *node, rclc_executor_t *executor)
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

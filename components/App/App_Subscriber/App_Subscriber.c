#include "App_Subscriber.h"

#include "esp_log.h"
#include "geometry_msgs/msg/twist.h"
#include "PID.h"
#include "rclc/subscription.h"
#include "rosidl_runtime_c/message_type_support_struct.h"

#define APP_CMD_VEL_TOPIC              "cmd_vel"
#define APP_WHEEL_CIRCUMFERENCE_M      0.1508f
#define APP_LINEAR_SPEED_MAX_MPS       1.0f
#define APP_ANGULAR_SPEED_MAX_RADPS    5.0f

static const char *TAG = "App_Subscriber";

static rcl_subscription_t s_twist_subscriber;
static geometry_msgs__msg__Twist s_twist_msg;

static float App_LimitFloat(float value, float min, float max)
{
    if (value > max)
    {
        return max;
    }
    if (value < min)
    {
        return min;
    }
    return value;
}

static float App_LinearMpsToRpm(float linear_mps)
{
    return linear_mps * 60.0f / APP_WHEEL_CIRCUMFERENCE_M;
}

static void App_TwistCallback(const void *msgin)
{
    const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;

    float linear_mps = App_LimitFloat((float)msg->linear.x,
                                      -APP_LINEAR_SPEED_MAX_MPS,
                                      APP_LINEAR_SPEED_MAX_MPS);
    float angular_radps = App_LimitFloat((float)msg->angular.z,
                                         -APP_ANGULAR_SPEED_MAX_RADPS,
                                         APP_ANGULAR_SPEED_MAX_RADPS);

    Speed_PID.target = App_LinearMpsToRpm(linear_mps);
    Angular_PID.target = angular_radps;

    ESP_LOGI(TAG, "cmd_vel linear: %.3f m/s, angular: %.3f rad/s", linear_mps, angular_radps);
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

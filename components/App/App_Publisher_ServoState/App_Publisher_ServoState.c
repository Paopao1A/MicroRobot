#include "App_Publisher_ServoState.h"

#include <stdio.h>

#include "Task_ServoControl.h"
#include "geometry_msgs/msg/vector3.h"
#include "rclc/publisher.h"
#include "rosidl_runtime_c/message_type_support_struct.h"

static rcl_publisher_t s_servo_state_publisher;
static rcl_timer_t s_servo_state_timer;
static geometry_msgs__msg__Vector3 s_servo_state_msg;

static void App_Publisher_ServoStateTimerCallback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void)last_call_time;

    if (timer == NULL)
    {
        return;
    }

    Task_ServoControl_State_t State;
    Task_ServoControl_GetState(&State);

    s_servo_state_msg.x = State.Pan_Deg;
    s_servo_state_msg.y = State.Tilt_Deg;
    s_servo_state_msg.z = 0.0;

    rcl_ret_t ret = rcl_publish(&s_servo_state_publisher, &s_servo_state_msg, NULL);
    (void)ret;
}

rcl_ret_t App_Publisher_ServoState_Init(rcl_node_t *node,
                                        rclc_executor_t *executor,
                                        rclc_support_t *support)
{
    rcl_ret_t ret = RCL_RET_OK;

    if ((node == NULL) || (executor == NULL) || (support == NULL))
    {
        return RCL_RET_INVALID_ARGUMENT;
    }

    ret = rclc_publisher_init_default(&s_servo_state_publisher,
                                      node,
                                      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Vector3),
                                      APP_SERVO_STATE_TOPIC);
    if (ret != RCL_RET_OK)
    {
        printf("servo_state publisher init failed: %d\n", (int)ret);
        return ret;
    }

    ret = rclc_timer_init_default(&s_servo_state_timer,
                                  support,
                                  RCL_MS_TO_NS(APP_SERVO_STATE_PUBLISH_PERIOD_MS),
                                  App_Publisher_ServoStateTimerCallback);
    if (ret != RCL_RET_OK)
    {
        printf("servo_state timer init failed: %d\n", (int)ret);
        return ret;
    }

    ret = rclc_executor_add_timer(executor, &s_servo_state_timer);
    if (ret != RCL_RET_OK)
    {
        printf("servo_state executor add timer failed: %d\n", (int)ret);
    }

    return ret;
}

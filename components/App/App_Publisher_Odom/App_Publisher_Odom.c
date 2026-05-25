#include "App_Publisher_Odom.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "Task_Odometry.h"
#include "nav_msgs/msg/detail/odometry__functions.h"
#include "nav_msgs/msg/odometry.h"
#include "rclc/publisher.h"
#include "rmw_microros/time_sync.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/string_functions.h"

static rcl_publisher_t s_odom_publisher;
static rcl_timer_t s_odom_timer;
static nav_msgs__msg__Odometry s_odom_msg;

static void App_Publisher_OdomEulerToQuat(float Roll, float Pitch, float Yaw, float Q[4])
{
    float Cy = cosf(Yaw * 0.5f);
    float Sy = sinf(Yaw * 0.5f);
    float Cp = cosf(Pitch * 0.5f);
    float Sp = sinf(Pitch * 0.5f);
    float Cr = cosf(Roll * 0.5f);
    float Sr = sinf(Roll * 0.5f);

    Q[0] = Cy * Cp * Cr + Sy * Sp * Sr;
    Q[1] = Cy * Cp * Sr - Sy * Sp * Cr;
    Q[2] = Sy * Cp * Sr + Cy * Sp * Cr;
    Q[3] = Sy * Cp * Cr - Cy * Sp * Sr;
}

static void App_Publisher_OdomSetStamp(void)
{
    int64_t Time_Ns = rmw_uros_epoch_nanos();

    if (Time_Ns < 0)
    {
        Time_Ns = 0;
    }

    s_odom_msg.header.stamp.sec = (int32_t)(Time_Ns / 1000000000LL);
    s_odom_msg.header.stamp.nanosec = (uint32_t)(Time_Ns % 1000000000LL);
}

static void App_Publisher_OdomFillMsg(const Task_Odometry_Data_t *Data)
{
    float Q[4] = {0.0f};
    App_Publisher_OdomEulerToQuat(0.0f, 0.0f, Data->Yaw_Rad, Q);

    App_Publisher_OdomSetStamp();
    s_odom_msg.pose.pose.position.x = Data->X_M;
    s_odom_msg.pose.pose.position.y = Data->Y_M;
    s_odom_msg.pose.pose.position.z = 0.0;
    s_odom_msg.pose.pose.orientation.x = Q[1];
    s_odom_msg.pose.pose.orientation.y = Q[2];
    s_odom_msg.pose.pose.orientation.z = Q[3];
    s_odom_msg.pose.pose.orientation.w = Q[0];

    s_odom_msg.twist.twist.linear.x = Data->Linear_X_Mps;
    s_odom_msg.twist.twist.linear.y = Data->Linear_Y_Mps;
    s_odom_msg.twist.twist.linear.z = 0.0;
    s_odom_msg.twist.twist.angular.x = 0.0;
    s_odom_msg.twist.twist.angular.y = 0.0;
    s_odom_msg.twist.twist.angular.z = Data->Angular_Z_Radps;
}

static void App_Publisher_OdomTimerCallback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void)last_call_time;

    if (timer == NULL)
    {
        return;
    }

    Task_Odometry_Data_t Odom_Data;
    Task_Odometry_GetData(&Odom_Data);
    App_Publisher_OdomFillMsg(&Odom_Data);
    rcl_ret_t ret = rcl_publish(&s_odom_publisher, &s_odom_msg, NULL);
    (void)ret;
}

rcl_ret_t App_Publisher_Odom_Init(rcl_node_t *node,
                                  rclc_executor_t *executor,
                                  rclc_support_t *support)
{
    rcl_ret_t ret = RCL_RET_OK;

    if ((node == NULL) || (executor == NULL) || (support == NULL))
    {
        return RCL_RET_INVALID_ARGUMENT;
    }

    nav_msgs__msg__Odometry__init(&s_odom_msg);//初始化里程计消息

    //设置里程计消息的子帧ID
    rosidl_runtime_c__String__assign(&s_odom_msg.header.frame_id, APP_ODOM_FRAME_ID);
    rosidl_runtime_c__String__assign(&s_odom_msg.child_frame_id, APP_ODOM_CHILD_FRAME_ID);
   
    //设置里程计消息的协方差矩阵
    memset(s_odom_msg.pose.covariance, 0, sizeof(s_odom_msg.pose.covariance));
    memset(s_odom_msg.twist.covariance, 0, sizeof(s_odom_msg.twist.covariance));
    s_odom_msg.pose.covariance[0] = 0.001;
    s_odom_msg.pose.covariance[7] = 0.001;
    s_odom_msg.pose.covariance[35] = 0.001;
    s_odom_msg.twist.covariance[0] = 0.0001;
    s_odom_msg.twist.covariance[7] = 0.0001;
    s_odom_msg.twist.covariance[35] = 0.0001;

    ret = rclc_publisher_init_default(&s_odom_publisher,
                                      node,
                                      ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
                                      APP_ODOM_RAW_TOPIC);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    ret = rclc_timer_init_default(&s_odom_timer,
                                  support,
                                  RCL_MS_TO_NS(APP_ODOM_PUBLISH_PERIOD_MS),
                                  App_Publisher_OdomTimerCallback);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    return rclc_executor_add_timer(executor, &s_odom_timer);
}

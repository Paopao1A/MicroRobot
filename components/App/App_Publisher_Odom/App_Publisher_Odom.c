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

static rcl_publisher_t s_odom_publisher;//里程计发布者
static rcl_timer_t s_odom_timer;//里程计定时器
static nav_msgs__msg__Odometry s_odom_msg;//需要发布的里程计消息

//将欧拉角转换为四元数，roll：侧倾，pitch：俯仰，yaw：横摆角度
static void App_Publisher_OdomEulerToQuat(float Roll, float Pitch, float Yaw, float Q[4])
{
    float Cy = cosf(Yaw * 0.5f);
    float Sy = sinf(Yaw * 0.5f);
    float Cp = cosf(Pitch * 0.5f);
    float Sp = sinf(Pitch * 0.5f);
    float Cr = cosf(Roll * 0.5f);
    float Sr = sinf(Roll * 0.5f);

    Q[0] = Cy * Cp * Cr + Sy * Sp * Sr;//w
    Q[1] = Cy * Cp * Sr - Sy * Sp * Cr;//x
    Q[2] = Sy * Cp * Sr + Cy * Sp * Cr;//y
    Q[3] = Sy * Cp * Cr - Cy * Sp * Sr;//z
}

//设置里程计消息的时间戳
static void App_Publisher_OdomSetStamp(void)
{
    int64_t Time_Ns = rmw_uros_epoch_nanos();//获取当前时间戳，单位：纳秒

    if (Time_Ns < 0)
    {
        Time_Ns = 0;
    }

    s_odom_msg.header.stamp.sec = (int32_t)(Time_Ns / 1000000000LL);//转换为秒
    s_odom_msg.header.stamp.nanosec = (uint32_t)(Time_Ns % 1000000000LL);
}

//填充里程计消息
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
    Task_Odometry_GetData(&Odom_Data);//获取里程计数据
    App_Publisher_OdomFillMsg(&Odom_Data);//填充里程计消息
    rcl_ret_t ret = rcl_publish(&s_odom_publisher, &s_odom_msg, NULL);//发布里程计消息
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
    rosidl_runtime_c__String__assign(&s_odom_msg.header.frame_id, APP_ODOM_FRAME_ID);//设置odom数据所在的父坐标系，里程计坐标系用于描述小车从开机开始之后实时的位置和姿态
    rosidl_runtime_c__String__assign(&s_odom_msg.child_frame_id, APP_ODOM_CHILD_FRAME_ID);//base_link机器人底盘坐标系（机器人本体），挂载着挂载 IMU、雷达、相机等传感器。作为机器人模型根部。base_footprint是机器人在地面上的投影坐标系,也就是没有z，roll和pitch，小车只做平面运动，所以base_footprint合适点
    //base_link和base_footprint是一个级别的，只不过一个是3D一个是2D，传感器等等都是挂在base_link上的，后续肯定还会用到base_link坐标系

   
    //设置里程计消息的协方差矩阵，covariance 是 6x6 矩阵，用一维数组存储，长度是 36：
    //[x, y, z, roll, pitch, yaw]，都在矩阵的对角线位置，因此x是[0], y是[7], yaw是[35]
    memset(s_odom_msg.pose.covariance, 0, sizeof(s_odom_msg.pose.covariance));//初始化协方差矩阵为0，memset就是把指定的值 "0" 复制到 "covariance" 所指向的内存区域的前 “sizeof(s_odom_msg.pose.covariance)” 个字节中
    memset(s_odom_msg.twist.covariance, 0, sizeof(s_odom_msg.twist.covariance));//初始化协方差矩阵为0
    s_odom_msg.pose.covariance[0] = 0.001;//设置x轴位置的协方差为0.001
    s_odom_msg.pose.covariance[7] = 0.001;//设置y轴位置的协方差为0.001
    s_odom_msg.pose.covariance[35] = 0.001;//设置yaw的协方差为0.001
    s_odom_msg.twist.covariance[0] = 0.0001;//设置x轴线速度的协方差为0.0001
    s_odom_msg.twist.covariance[7] = 0.0001;//设置y轴线速度的协方差为0.0001
    s_odom_msg.twist.covariance[35] = 0.0001;//设置yaw的协方差为0.0001

    /* topic: odom_raw               话题名称
       type:  nav_msgs/msg/Odometry  消息类型
       node:  micro_robot            节点名称 */
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

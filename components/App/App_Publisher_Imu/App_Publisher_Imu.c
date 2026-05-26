#include "App_Publisher_Imu.h"

#include <stdint.h>
#include <string.h>

#include "Task_IMU.h"
#include "rclc/publisher.h"
#include "rmw_microros/time_sync.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/string_functions.h"
#include "sensor_msgs/msg/imu.h"

static rcl_publisher_t s_imu_publisher;
static rcl_timer_t s_imu_timer;
static sensor_msgs__msg__Imu s_imu_msg;

static void App_Publisher_ImuSetStamp(void)
{
    int64_t Time_Ns = rmw_uros_epoch_nanos();
    if (Time_Ns < 0)
    {
        Time_Ns = 0;
    }

    s_imu_msg.header.stamp.sec = (int32_t)(Time_Ns / 1000000000LL);
    s_imu_msg.header.stamp.nanosec = (uint32_t)(Time_Ns % 1000000000LL);
}

//填充IMU消息
static void App_Publisher_ImuFillMsg(const Task_IMU_Snapshot_t *Snapshot)
{
    App_Publisher_ImuSetStamp();//设置时间戳

    s_imu_msg.orientation.x = 0.0;//不提供姿态信息，后续上位机会融合里程计和imu进行解算姿态
    s_imu_msg.orientation.y = 0.0;
    s_imu_msg.orientation.z = 0.0;
    s_imu_msg.orientation.w = 1.0;

    s_imu_msg.angular_velocity.x = Snapshot->Gyro_Radps[0];
    s_imu_msg.angular_velocity.y = Snapshot->Gyro_Radps[1];
    s_imu_msg.angular_velocity.z = Snapshot->Gyro_Radps[2];

    s_imu_msg.linear_acceleration.x = Snapshot->Accel_Ms2[0];
    s_imu_msg.linear_acceleration.y = Snapshot->Accel_Ms2[1];
    s_imu_msg.linear_acceleration.z = Snapshot->Accel_Ms2[2];
}

static void App_Publisher_ImuTimerCallback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void)last_call_time;

    if (timer == NULL)
    {
        return;
    }

    Task_IMU_Snapshot_t Snapshot;
    Task_IMU_GetSnapshot(&Snapshot);
    if (!Snapshot.Is_Valid)
    {
        return;
    }

    App_Publisher_ImuFillMsg(&Snapshot);
    rcl_ret_t ret = rcl_publish(&s_imu_publisher, &s_imu_msg, NULL);
    (void)ret;
}

rcl_ret_t App_Publisher_Imu_Init(rcl_node_t *node,
                                 rclc_executor_t *executor,
                                 rclc_support_t *support)
{
    rcl_ret_t ret = RCL_RET_OK;

    if ((node == NULL) || (executor == NULL) || (support == NULL))
    {
        return RCL_RET_INVALID_ARGUMENT;
    }

    sensor_msgs__msg__Imu__init(&s_imu_msg);
    rosidl_runtime_c__String__assign(&s_imu_msg.header.frame_id, APP_IMU_FRAME_ID);//imu_link坐标系名称

    memset(s_imu_msg.orientation_covariance, 0, sizeof(s_imu_msg.orientation_covariance));
    memset(s_imu_msg.angular_velocity_covariance, 0, sizeof(s_imu_msg.angular_velocity_covariance));
    memset(s_imu_msg.linear_acceleration_covariance, 0, sizeof(s_imu_msg.linear_acceleration_covariance));

    s_imu_msg.orientation_covariance[0] = -1.0;//不提供真实姿态信息，也就是说姿态orientation数据是直接忽略的
    s_imu_msg.angular_velocity_covariance[0] = 0.0001;
    s_imu_msg.angular_velocity_covariance[4] = 0.0001;
    s_imu_msg.angular_velocity_covariance[8] = 0.0001;
    s_imu_msg.linear_acceleration_covariance[0] = 0.01;
    s_imu_msg.linear_acceleration_covariance[4] = 0.01;
    s_imu_msg.linear_acceleration_covariance[8] = 0.01;

    ret = rclc_publisher_init_default(&s_imu_publisher,
                                      node,
                                      ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
                                      APP_IMU_RAW_TOPIC);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    ret = rclc_timer_init_default(&s_imu_timer,
                                  support,
                                  RCL_MS_TO_NS(APP_IMU_PUBLISH_PERIOD_MS),
                                  App_Publisher_ImuTimerCallback);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    return rclc_executor_add_timer(executor, &s_imu_timer);
}

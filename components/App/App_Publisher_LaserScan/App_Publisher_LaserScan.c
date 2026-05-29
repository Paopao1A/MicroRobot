#include "App_Publisher_LaserScan.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "Task_Lidar.h"
#include "rclc/publisher.h"
#include "rmw_microros/time_sync.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/string_functions.h"
#include "sensor_msgs/msg/detail/laser_scan__functions.h"
#include "sensor_msgs/msg/laser_scan.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static rcl_publisher_t s_laser_scan_publisher;
static rcl_timer_t s_laser_scan_timer;
static sensor_msgs__msg__LaserScan s_laser_scan_msg;
static float s_laser_scan_ranges[APP_LASER_SCAN_POINT_NUM] = {0};
static float s_laser_scan_intensities[APP_LASER_SCAN_POINT_NUM] = {0};

static void App_Publisher_LaserScanSetStamp(void)
{
    int64_t Time_Ns = rmw_uros_epoch_nanos();
    if (Time_Ns < 0)
    {
        Time_Ns = 0;
    }

    s_laser_scan_msg.header.stamp.sec = (int32_t)(Time_Ns / 1000000000LL);
    s_laser_scan_msg.header.stamp.nanosec = (uint32_t)(Time_Ns % 1000000000LL);
}

//“方向反转 + 180 度安装角补偿”映射索引，一般用于处理雷达 0 度方向和车体 base_link 前方不一致的问题。
static uint16_t App_Publisher_LaserScanMapIndex(uint16_t Index)
{
    uint16_t SourceIndex = (APP_LASER_SCAN_POINT_NUM - Index) % APP_LASER_SCAN_POINT_NUM;

    if (SourceIndex >= 180)
    {
        SourceIndex = (SourceIndex - 180) % APP_LASER_SCAN_POINT_NUM;
    }
    else
    {
        SourceIndex = (SourceIndex + 180) % APP_LASER_SCAN_POINT_NUM;
    }

    return SourceIndex;
}

static void App_Publisher_LaserScanFillMsg(const Task_Lidar_Scan_t *Scan)
{
    App_Publisher_LaserScanSetStamp();//设置时间戳

    for (uint16_t i = 0; i < APP_LASER_SCAN_POINT_NUM; i++)
    {
        uint16_t SourceIndex = App_Publisher_LaserScanMapIndex(i);//映射索引
        float Range_M = (float)Scan->Scan.Points[SourceIndex].Distance_Mm / 1000.0f;//距离

        if (Range_M < APP_LASER_SCAN_RANGE_MIN_M || Range_M > APP_LASER_SCAN_RANGE_MAX_M)
        {
            s_laser_scan_ranges[i] = 0.0f;
        }
        else
        {
            s_laser_scan_ranges[i] = Range_M;
        }
        s_laser_scan_intensities[i] = (float)Scan->Scan.Points[SourceIndex].Intensity;
    }
}

//这里的定时回调不是硬件中断，是freertos任务，所以数据需要进行竟态保护
static void App_Publisher_LaserScanTimerCallback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void)last_call_time;

    if (timer == NULL)
    {
        return;
    }

    Task_Lidar_Scan_t Scan;
    Task_Lidar_GetScan(&Scan);
    if (!Scan.Is_Valid)
    {
        return;
    }

    App_Publisher_LaserScanFillMsg(&Scan);//填充消息
    rcl_ret_t ret = rcl_publish(&s_laser_scan_publisher, &s_laser_scan_msg, NULL);//发布消息
    (void)ret;
}

static void App_Publisher_LaserScanInitMsg(void)
{
    sensor_msgs__msg__LaserScan__init(&s_laser_scan_msg);

    rosidl_runtime_c__String__assign(&s_laser_scan_msg.header.frame_id, APP_LASER_SCAN_FRAME_ID);
    s_laser_scan_msg.angle_min = -(float)M_PI;//最小角度
    s_laser_scan_msg.angle_max = (float)M_PI;//最大角度
    s_laser_scan_msg.angle_increment =  (float)M_PI / 180.0f;//角度增量
    s_laser_scan_msg.time_increment = 0.0f;//时间增量
    s_laser_scan_msg.scan_time = (float)APP_LASER_SCAN_PUBLISH_PERIOD_MS / 1000.0f;//扫描时间
    s_laser_scan_msg.range_min = APP_LASER_SCAN_RANGE_MIN_M;//最小距离
    s_laser_scan_msg.range_max = APP_LASER_SCAN_RANGE_MAX_M;//最大距离

    memset(s_laser_scan_ranges, 0, sizeof(s_laser_scan_ranges));
    memset(s_laser_scan_intensities, 0, sizeof(s_laser_scan_intensities));
    s_laser_scan_msg.ranges.data = s_laser_scan_ranges;//距离数组
    s_laser_scan_msg.ranges.size = APP_LASER_SCAN_POINT_NUM;//距离数组大小
    s_laser_scan_msg.ranges.capacity = APP_LASER_SCAN_POINT_NUM;//距离数组容量
    s_laser_scan_msg.intensities.data = s_laser_scan_intensities;//强度数组
    s_laser_scan_msg.intensities.size = APP_LASER_SCAN_POINT_NUM;//强度数组大小
    s_laser_scan_msg.intensities.capacity = APP_LASER_SCAN_POINT_NUM;//强度数组容量
}

rcl_ret_t App_Publisher_LaserScan_Init(rcl_node_t *node,
                                       rclc_executor_t *executor,
                                       rclc_support_t *support)
{
    rcl_ret_t ret = RCL_RET_OK;

    if ((node == NULL) || (executor == NULL) || (support == NULL))
    {
        return RCL_RET_INVALID_ARGUMENT;
    }

    App_Publisher_LaserScanInitMsg();//初始化消息

    ret = rclc_publisher_init_default(&s_laser_scan_publisher,
                                      node,
                                      ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan),
                                      APP_LASER_SCAN_TOPIC);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    ret = rclc_timer_init_default(&s_laser_scan_timer,
                                  support,
                                  RCL_MS_TO_NS(APP_LASER_SCAN_PUBLISH_PERIOD_MS),
                                  App_Publisher_LaserScanTimerCallback);
    if (ret != RCL_RET_OK)
    {
        return ret;
    }

    return rclc_executor_add_timer(executor, &s_laser_scan_timer);
}

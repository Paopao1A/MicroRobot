#ifndef __APP_PUBLISHER_LASER_SCAN_H__
#define __APP_PUBLISHER_LASER_SCAN_H__

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"

#define APP_PUBLISHER_LASER_SCAN_HANDLE_NUM 1
#define APP_LASER_SCAN_TOPIC                "scan"
#define APP_LASER_SCAN_FRAME_ID             "laser_frame"
#define APP_LASER_SCAN_PUBLISH_PERIOD_MS    90
#define APP_LASER_SCAN_POINT_NUM            360
#define APP_LASER_SCAN_RANGE_MIN_M          0.12f
#define APP_LASER_SCAN_RANGE_MAX_M          8.0f

rcl_ret_t App_Publisher_LaserScan_Init(rcl_node_t *node,
                                       rclc_executor_t *executor,
                                       rclc_support_t *support);

#endif

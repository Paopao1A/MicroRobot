#ifndef __APP_PUBLISHER_IMU_H__
#define __APP_PUBLISHER_IMU_H__

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"

#define APP_PUBLISHER_IMU_HANDLE_NUM    1
#define APP_IMU_RAW_TOPIC               "imu_raw"
#define APP_IMU_FRAME_ID                "imu_link"
#define APP_IMU_PUBLISH_PERIOD_MS       20

rcl_ret_t App_Publisher_Imu_Init(rcl_node_t *node,
                                 rclc_executor_t *executor,
                                 rclc_support_t *support);

#endif

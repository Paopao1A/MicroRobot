#ifndef __APP_PUBLISHER_ODOM_H__
#define __APP_PUBLISHER_ODOM_H__

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"

#define APP_PUBLISHER_ODOM_HANDLE_NUM    1
#define APP_ODOM_RAW_TOPIC               "odom_raw"
#define APP_ODOM_FRAME_ID                "odom_frame"
#define APP_ODOM_CHILD_FRAME_ID          "base_footprint"
#define APP_ODOM_PUBLISH_PERIOD_MS       50

rcl_ret_t App_Publisher_Odom_Init(rcl_node_t *node,
                                  rclc_executor_t *executor,
                                  rclc_support_t *support);

#endif

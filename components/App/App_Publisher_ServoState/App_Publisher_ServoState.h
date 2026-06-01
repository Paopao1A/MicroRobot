#ifndef __APP_PUBLISHER_SERVO_STATE_H__
#define __APP_PUBLISHER_SERVO_STATE_H__

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"

#define APP_PUBLISHER_SERVO_STATE_HANDLE_NUM    1
#define APP_SERVO_STATE_TOPIC                   "servo_state"
#define APP_SERVO_STATE_PUBLISH_PERIOD_MS       100

rcl_ret_t App_Publisher_ServoState_Init(rcl_node_t *node,
                                        rclc_executor_t *executor,
                                        rclc_support_t *support);

#endif

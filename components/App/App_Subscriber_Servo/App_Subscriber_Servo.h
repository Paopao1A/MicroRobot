#ifndef __APP_SUBSCRIBER_SERVO_H__
#define __APP_SUBSCRIBER_SERVO_H__

#include <stdbool.h>

#include "rcl/rcl.h"
#include "rclc/executor.h"

#define APP_SUBSCRIBER_SERVO_HANDLE_NUM 1

#define APP_SERVO_CMD_TOPIC              "servo_cmd"
#define APP_SERVO_CMD_TIMEOUT_MS         3000
#define APP_SERVO_CMD_LOG_ENABLE         0

typedef struct
{
    float Pan_Deg;
    float Tilt_Deg;
    bool Is_Timeout;
} App_Servo_Target_t;

rcl_ret_t App_Subscriber_Servo_Init(rcl_node_t *node, rclc_executor_t *executor);
void App_Subscriber_Servo_GetTarget(App_Servo_Target_t *Target);

#endif

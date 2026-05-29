#ifndef __APP_SUBSCRIBER_TWIST_H__
#define __APP_SUBSCRIBER_TWIST_H__

#include <stdbool.h>

#include "rcl/rcl.h"
#include "rclc/executor.h"

#define APP_SUBSCRIBER_TWIST_HANDLE_NUM 1

#define APP_CMD_VEL_TOPIC              "cmd_vel"//速度角速度目标话题
#define APP_WHEEL_CIRCUMFERENCE_M      0.1508f //轮子半径，单位：米
#define APP_SPEED_TARGET_MAX_RPM       200.0f   //最大转速，单位：RPM
#define APP_ANGULAR_TARGET_MAX_RADPS   2.0f    //最大角速度，单位：弧度/秒
#define APP_CMD_VEL_TIMEOUT_MS         3000  //超时时间，单位：毫秒
#define APP_CMD_VEL_LOG_ENABLE         0

typedef struct
{
    float Speed_RPM;
    float Angular_Radps;
    bool Is_Timeout;
} App_CmdVel_Target_t;

rcl_ret_t App_Subscriber_Twist_Init(rcl_node_t *node, rclc_executor_t *executor);
void App_Subscriber_Twist_GetCmdVelTarget(App_CmdVel_Target_t *Target);

#endif

#ifndef __APP_SUBSCRIBER_H__
#define __APP_SUBSCRIBER_H__ 

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include <stdbool.h>

#define APP_SUBSCRIBER_HANDLE_NUM 1

#define APP_CMD_VEL_TOPIC              "cmd_vel"//速度和角速度目标话题
#define APP_WHEEL_CIRCUMFERENCE_M      0.1508f //轮子周长，单位：米
#define APP_SPEED_TARGET_MAX_RPM       200.0f   //最大速度目标，单位：RPM
#define APP_ANGULAR_TARGET_MAX_RADPS   2.0f    //最大角速度目标，单位：弧度/秒
#define APP_CMD_VEL_TIMEOUT_MS         10000

typedef struct
{
    float Speed_RPM;
    float Angular_Radps;
    bool Is_Timeout;
} App_CmdVel_Target_t;

rcl_ret_t App_Subscriber_Init(rcl_node_t *node, rclc_executor_t *executor);
void App_Subscriber_GetCmdVelTarget(App_CmdVel_Target_t *Target);

#endif

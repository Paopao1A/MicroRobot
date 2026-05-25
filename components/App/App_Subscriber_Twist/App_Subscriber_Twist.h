#ifndef __APP_SUBSCRIBER_TWIST_H__
#define __APP_SUBSCRIBER_TWIST_H__

#include <stdbool.h>

#include "rcl/rcl.h"
#include "rclc/executor.h"

#define APP_SUBSCRIBER_TWIST_HANDLE_NUM 1

#define APP_CMD_VEL_TOPIC              "cmd_vel"//閫熷害鍜岃閫熷害鐩爣璇濋
#define APP_WHEEL_CIRCUMFERENCE_M      0.1508f //杞瓙鍛ㄩ暱锛屽崟浣嶏細绫?
#define APP_SPEED_TARGET_MAX_RPM       200.0f   //鏈€澶ч€熷害鐩爣锛屽崟浣嶏細RPM
#define APP_ANGULAR_TARGET_MAX_RADPS   2.0f    //鏈€澶ц閫熷害鐩爣锛屽崟浣嶏細寮у害/绉?
#define APP_CMD_VEL_TIMEOUT_MS         3000  //閫熷害鍜岃閫熷害鐩爣瓒呮椂鏃堕棿锛屽崟浣嶏細姣

typedef struct
{
    float Speed_RPM;
    float Angular_Radps;
    bool Is_Timeout;
} App_CmdVel_Target_t;

rcl_ret_t App_Subscriber_Twist_Init(rcl_node_t *node, rclc_executor_t *executor);
void App_Subscriber_Twist_GetCmdVelTarget(App_CmdVel_Target_t *Target);

#endif

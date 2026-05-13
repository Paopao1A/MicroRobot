#ifndef __APP_SUBSCRIBER_H__
#define __APP_SUBSCRIBER_H__ 

#include "rcl/rcl.h"
#include "rclc/executor.h"

#define APP_SUBSCRIBER_HANDLE_NUM 1

rcl_ret_t App_Subscriber_Init(rcl_node_t *node, rclc_executor_t *executor);

#endif

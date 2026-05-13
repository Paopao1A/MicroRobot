#include "App_Init.h"

#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rcl/rcl.h"
#include "rclc/rclc.h"
#include "rclc/executor.h"
#include "rmw_microros/rmw_microros.h"
#include "App_Subscriber.h"
#include "WIFI.h"

#ifndef CONFIG_MICRO_ROS_APP_STACK
#define CONFIG_MICRO_ROS_APP_STACK 16000
#endif

#ifndef CONFIG_MICRO_ROS_APP_TASK_PRIO
#define CONFIG_MICRO_ROS_APP_TASK_PRIO 5
#endif

#ifndef CONFIG_MICRO_ROS_DOMAIN_ID
#define CONFIG_MICRO_ROS_DOMAIN_ID 20
#endif

#ifndef CONFIG_MICRO_ROS_NAMESPACE
#define CONFIG_MICRO_ROS_NAMESPACE ""
#endif

#define RCCHECK(fn)                                                              \
    do                                                                           \
    {                                                                            \
        rcl_ret_t temp_rc = (fn);                                                \
        if (temp_rc != RCL_RET_OK)                                               \
        {                                                                        \
            printf("micro-ROS failed on line %d: %d\n", __LINE__, (int)temp_rc); \
            vTaskDelete(NULL);                                                   \
        }                                                                        \
    } while (0)

#define ROS_NAMESPACE  CONFIG_MICRO_ROS_NAMESPACE
#define ROS_DOMAIN_ID  CONFIG_MICRO_ROS_DOMAIN_ID
#define ROS_AGENT_IP   CONFIG_MICRO_ROS_AGENT_IP
#define ROS_AGENT_PORT CONFIG_MICRO_ROS_AGENT_PORT

static const char *TAG = "App_Init";
static TaskHandle_t s_micro_ros_task_handle = NULL;

static void App_MicroRosTask(void *arg)
{
    if (!WIFI_IsConnected())
    {
        ESP_LOGI(TAG, "Waiting for WiFi connection");
        WIFI_Init();
    }

    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_options, allocator));
    RCCHECK(rcl_init_options_set_domain_id(&init_options, ROS_DOMAIN_ID));

    rmw_init_options_t *rmw_options = rcl_init_options_get_rmw_init_options(&init_options);
    RCCHECK(rmw_uros_options_set_udp_address(ROS_AGENT_IP, ROS_AGENT_PORT, rmw_options));

    while (rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator) != RCL_RET_OK)
    {
        ESP_LOGW(TAG, "Connecting micro-ROS agent %s:%s", ROS_AGENT_IP, ROS_AGENT_PORT);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI(TAG, "Connected micro-ROS agent %s:%s", ROS_AGENT_IP, ROS_AGENT_PORT);

    rcl_node_t node;
    RCCHECK(rclc_node_init_default(&node, "micro_robot", ROS_NAMESPACE, &support));
    ESP_LOGI(TAG, "micro-ROS node initialized");

    rclc_executor_t executor;
    RCCHECK(rclc_executor_init(&executor,
                               &support.context,
                               APP_SUBSCRIBER_HANDLE_NUM,
                               &allocator));
    RCCHECK(App_Subscriber_Init(&node, &executor));
    ESP_LOGI(TAG, "micro-ROS subscribers initialized");

    while (1)
    {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void App_Init(void)
{
    if (s_micro_ros_task_handle != NULL)
    {
        return;
    }

    xTaskCreate(App_MicroRosTask,
                "App_MicroRosTask",
                CONFIG_MICRO_ROS_APP_STACK,
                NULL,
                CONFIG_MICRO_ROS_APP_TASK_PRIO,
                &s_micro_ros_task_handle);

    if (s_micro_ros_task_handle == NULL)
    {
        ESP_LOGE(TAG, "Create App_MicroRosTask failed");
    }
}

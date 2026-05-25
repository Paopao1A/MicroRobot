#include "Task_IMU.h"

#include "IMU.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#define TASK_IMU_STACK_SIZE 4096
#define TASK_IMU_PRIORITY   7

static portMUX_TYPE s_IMU_Lock = portMUX_INITIALIZER_UNLOCKED;
static Task_IMU_Snapshot_t s_IMU_Snapshot = {0};//IMU数据快照

static void Task_IMU(void *pvParameters);

void Task_IMU_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_IMU,
                "Task_IMU",
                TASK_IMU_STACK_SIZE,
                NULL,
                TASK_IMU_PRIORITY,
                &xTaskHandle);
}

//获取IMU全部数据快照
void Task_IMU_GetSnapshot(Task_IMU_Snapshot_t *Snapshot)
{
    if (Snapshot == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_IMU_Lock);
    *Snapshot = s_IMU_Snapshot;
    portEXIT_CRITICAL(&s_IMU_Lock);
}

//获取IMU角速度Z轴数据快照
float Task_IMU_GetAngularZRadps(void)
{
    float Angular_Z_Radps = 0.0f;

    portENTER_CRITICAL(&s_IMU_Lock);
    if (s_IMU_Snapshot.Is_Valid)
    {
        Angular_Z_Radps = s_IMU_Snapshot.Gyro_Radps[2];
    }
    portEXIT_CRITICAL(&s_IMU_Lock);

    return Angular_Z_Radps;
}

static void Task_IMU(void *pvParameters)
{
    (void)pvParameters;

    TickType_t LastWakeTime = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(TASK_IMU_PERIOD_MS));

        Task_IMU_Snapshot_t Snapshot = {0};
        int ret = IMU_Update_All(Snapshot.Gyro_Raw,
                                 Snapshot.Gyro_Radps,
                                 Snapshot.Accel_Raw,
                                 Snapshot.Accel_Ms2);

        Snapshot.Is_Valid = (ret == 0);
        Snapshot.Tick_Ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

        portENTER_CRITICAL(&s_IMU_Lock);
        Snapshot.Update_Count = s_IMU_Snapshot.Update_Count + 1;
        s_IMU_Snapshot = Snapshot;
        portEXIT_CRITICAL(&s_IMU_Lock);
    }
}

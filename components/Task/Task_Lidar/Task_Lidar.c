#include "Task_Lidar.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "UART.h"

#define TASK_LIDAR_STACK_SIZE 4096
#define TASK_LIDAR_PRIORITY   6

static SemaphoreHandle_t s_Lidar_Mutex = NULL;
static Task_Lidar_Scan_t s_Lidar_Scan = {0};

static void Task_Lidar(void *pvParameters);

void Task_Lidar_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    if (s_Lidar_Mutex == NULL)
    {
        s_Lidar_Mutex = xSemaphoreCreateMutex();
        if (s_Lidar_Mutex == NULL)
        {
            return;
        }
    }

    xTaskCreate(Task_Lidar,
                "Task_Lidar",
                TASK_LIDAR_STACK_SIZE,
                NULL,
                TASK_LIDAR_PRIORITY,
                &xTaskHandle);
}

//获取当前扫描数据的快照，用于APP层调用
void Task_Lidar_GetScan(Task_Lidar_Scan_t *Scan)
{
    if (Scan == NULL)
    {
        return;
    }

    if (s_Lidar_Mutex == NULL)
    {
        return;
    }

    if (xSemaphoreTake(s_Lidar_Mutex, portMAX_DELAY) == pdTRUE)
    {
        *Scan = s_Lidar_Scan;
        xSemaphoreGive(s_Lidar_Mutex);
    }
}

static void Task_Lidar_UpdateSnapshot(void)
{
    LIDAR_Scan_t LidarScan = {0};

    LIDAR_GetScan(&LidarScan);//获取当前扫描数据
    if (s_Lidar_Mutex == NULL)
    {
        return;
    }

    if (xSemaphoreTake(s_Lidar_Mutex, portMAX_DELAY) == pdTRUE)//互斥锁保护
    {
        s_Lidar_Scan.Scan = LidarScan;
        s_Lidar_Scan.Is_Valid = true;
        s_Lidar_Scan.Tick_Ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        s_Lidar_Scan.Update_Count++;
        xSemaphoreGive(s_Lidar_Mutex);
    }
}

static void Task_Lidar(void *pvParameters)
{
    (void)pvParameters;

    uint8_t ReadBuffer[TASK_LIDAR_READ_BUFFER_SIZE] = {0};
    TickType_t LastSnapshotTick = 0;

    while (1)
    {
        int ReadLen = UART_ReadBytes(ReadBuffer, sizeof(ReadBuffer), 5);
        if (ReadLen > 0)
        {
            for (int i = 0; i < ReadLen; i++)
            {
                LIDAR_ReceiveByte(ReadBuffer[i]);//处理接收到的字节
            }
        }

        if (LIDAR_HasNewScan())//检查是否有新的扫描数据
        {
            LIDAR_ClearNewScan();
            TickType_t CurrentTick = xTaskGetTickCount();
            if ((CurrentTick - LastSnapshotTick) >= pdMS_TO_TICKS(20))
            {
                LastSnapshotTick = CurrentTick;
                Task_Lidar_UpdateSnapshot();
            }
        }
        else if (ReadLen <= 0)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

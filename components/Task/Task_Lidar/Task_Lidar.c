#include "Task_Lidar.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "UART.h"

#define TASK_LIDAR_STACK_SIZE 4096
#define TASK_LIDAR_PRIORITY   6

static portMUX_TYPE s_Lidar_Lock = portMUX_INITIALIZER_UNLOCKED;
static Task_Lidar_Scan_t s_Lidar_Scan = {0};

static void Task_Lidar(void *pvParameters);

void Task_Lidar_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_Lidar,
                "Task_Lidar",
                TASK_LIDAR_STACK_SIZE,
                NULL,
                TASK_LIDAR_PRIORITY,
                &xTaskHandle);
}

void Task_Lidar_GetScan(Task_Lidar_Scan_t *Scan)
{
    if (Scan == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_Lidar_Lock);
    *Scan = s_Lidar_Scan;
    portEXIT_CRITICAL(&s_Lidar_Lock);
}

static void Task_Lidar_UpdateSnapshot(void)
{
    LIDAR_Scan_t LidarScan = {0};
    Task_Lidar_Scan_t Scan = {0};

    LIDAR_GetScan(&LidarScan);
    for (uint16_t i = 0; i < TASK_LIDAR_SCAN_POINT_NUM; i++)
    {
        Scan.Distance_Mm[i] = LidarScan.Points[i].Distance_Mm;
        Scan.Intensity[i] = LidarScan.Points[i].Intensity;
    }
    Scan.Is_Valid = true;
    Scan.Tick_Ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    portENTER_CRITICAL(&s_Lidar_Lock);
    Scan.Update_Count = s_Lidar_Scan.Update_Count + 1;
    s_Lidar_Scan = Scan;
    portEXIT_CRITICAL(&s_Lidar_Lock);
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
                LIDAR_ReceiveByte(ReadBuffer[i]);
            }
        }

        if (LIDAR_HasNewScan())
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

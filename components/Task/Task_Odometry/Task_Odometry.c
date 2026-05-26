#include "Task_Odometry.h"

#include <math.h>
#include <stdbool.h>

#include "Encoder.h"
#include "Task_IMU.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#define TASK_ODOMETRY_STACK_SIZE  4096
#define TASK_ODOMETRY_PRIORITY    7

static portMUX_TYPE s_Odometry_Lock = portMUX_INITIALIZER_UNLOCKED;
static Task_Odometry_Data_t s_Odometry_Data = {0};

static void Task_Odometry(void *pvParameters);

//将编码器计数转换为距离，单位：米
static float Task_Odometry_CountToDistanceM(int Count)
{
    return (float)Count / ENCODER_COUNT_PER_REV * TASK_ODOMETRY_WHEEL_CIRCUM_M;
}

void Task_Odometry_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_Odometry,
                "Task_Odometry",
                TASK_ODOMETRY_STACK_SIZE,
                NULL,
                TASK_ODOMETRY_PRIORITY,
                &xTaskHandle);
}

void Task_Odometry_GetData(Task_Odometry_Data_t *Data)
{
    if (Data == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_Odometry_Lock);
    *Data = s_Odometry_Data;
    portEXIT_CRITICAL(&s_Odometry_Lock);
}

static void Task_Odometry(void *pvParameters)
{
    (void)pvParameters;

    TickType_t LastWakeTime = xTaskGetTickCount();
    TickType_t LastUpdateTick = LastWakeTime;
    bool Is_First_Update = true;
    int Last_Count[ENCODER_NUM] = {0};
    float X_M = 0.0f;
    float Y_M = 0.0f;
    float Yaw_Rad = 0.0f;

    while (1)
    {
        //等待指定时间，20ms更新一次里程计
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(TASK_ODOMETRY_PERIOD_MS));
        TickType_t CurrentTick = xTaskGetTickCount();

        //获取当前编码器计数
        int Current_Count[ENCODER_NUM] = {
            Encoder_Get_Count(ENCODER_ID_M1),
            Encoder_Get_Count(ENCODER_ID_M2),
            Encoder_Get_Count(ENCODER_ID_M3),
            Encoder_Get_Count(ENCODER_ID_M4),
        };

        if (Is_First_Update)
        {
            for (uint8_t i = 0; i < ENCODER_NUM; i++)
            {
                Last_Count[i] = Current_Count[i];
            }
            Is_First_Update = false;
            LastUpdateTick = CurrentTick;
            continue;//结束当前循环中的剩余代码执行，并直接跳转至下一次的循环条件判断
        }

        //计算编码器计数变化
        int Delta_Count[ENCODER_NUM] = {0};
        for (uint8_t i = 0; i < ENCODER_NUM; i++)
        {
            Delta_Count[i] = Current_Count[i] - Last_Count[i];
            Last_Count[i] = Current_Count[i];
        }

        //计算时间间隔
        TickType_t DeltaTick = CurrentTick - LastUpdateTick;
        LastUpdateTick = CurrentTick;
        if (DeltaTick == 0)
        {
            DeltaTick = pdMS_TO_TICKS(TASK_ODOMETRY_PERIOD_MS);
        }

        //时间间隔TickType_t类型转化为秒
        float Dt_S = (float)DeltaTick * (float)portTICK_PERIOD_MS / 1000.0f;
        //将编码器计数转换为距离
        float Wheel_Distance[ENCODER_NUM] = {
            Task_Odometry_CountToDistanceM(Delta_Count[0]),
            Task_Odometry_CountToDistanceM(Delta_Count[1]),
            Task_Odometry_CountToDistanceM(Delta_Count[2]),
            Task_Odometry_CountToDistanceM(Delta_Count[3]),
        };
        //计算轮速
        float Wheel_Speed[ENCODER_NUM] = {
            Wheel_Distance[0] / Dt_S,
            Wheel_Distance[1] / Dt_S,
            Wheel_Distance[2] / Dt_S,
            Wheel_Distance[3] / Dt_S,
        };

        //计算线速度
        float Linear_X_Mps = (Wheel_Speed[0] + Wheel_Speed[1] + Wheel_Speed[2] + Wheel_Speed[3]) / 4.0f;
        float Linear_Y_Mps = 0.0f;
        //计算角速度，这里没用传感器主要是I2C没有进行竞争态保护，就用一个额外的任务进行数据快照获取
        float Angular_Z_Radps = Task_IMU_GetAngularZRadps();
        //计算位姿
        float Cos_Yaw = cosf(Yaw_Rad);
        float Sin_Yaw = sinf(Yaw_Rad);
        X_M += (Linear_X_Mps * Cos_Yaw - Linear_Y_Mps * Sin_Yaw) * Dt_S;
        Y_M += (Linear_X_Mps * Sin_Yaw + Linear_Y_Mps * Cos_Yaw) * Dt_S;
        Yaw_Rad += Angular_Z_Radps * Dt_S;

        //更新里程计数据
        portENTER_CRITICAL(&s_Odometry_Lock);
        s_Odometry_Data.X_M = X_M;  //X坐标，单位：米
        s_Odometry_Data.Y_M = Y_M;  //Y坐标，单位：米
        s_Odometry_Data.Yaw_Rad = Yaw_Rad;  //当前横摆角度，单位：弧度
        s_Odometry_Data.Linear_X_Mps = Linear_X_Mps;    //X线速度，单位：米/秒
        s_Odometry_Data.Linear_Y_Mps = Linear_Y_Mps;    //Y线速度，单位：米/秒
        s_Odometry_Data.Angular_Z_Radps = Angular_Z_Radps;    //Z角速度，单位：弧度/秒
        s_Odometry_Data.Update_Count++;    //更新计数
        portEXIT_CRITICAL(&s_Odometry_Lock);
    }
}

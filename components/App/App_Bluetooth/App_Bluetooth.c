#include "App_Bluetooth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "App_Subscriber.h"
#include "Bluetooth.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

typedef struct
{
    bool SpeedKp;//是否配置了速度比例系数，下同
    bool SpeedKi;
    bool SpeedKd;
    bool AngularKp;
    bool AngularKi;
    bool AngularKd;
    float SpeedKpValue;//速度比例系数，下同
    float SpeedKiValue;
    float SpeedKdValue;
    float AngularKpValue;
    float AngularKiValue;
    float AngularKdValue;
} App_Bluetooth_PidConfig_t;

static const char *TAG = "App_Bluetooth";
static portMUX_TYPE s_Bluetooth_Lock = portMUX_INITIALIZER_UNLOCKED;//蓝牙锁，用于保护蓝牙数据的访问
static char s_Bluetooth_RxBuffer[APP_BLUETOOTH_PACKET_MAX_LEN * 2];//接收缓冲区，用于存储从 ESP32 接收的数据
static uint16_t s_Bluetooth_RxLen = 0;
static App_Bluetooth_Target_t s_Bluetooth_Target = {
    .Speed_RPM = 0.0f,
    .Angular_Radps = 0.0f,
    .Is_Timeout = false,
};
static App_Bluetooth_PidConfig_t s_Bluetooth_PidConfig = {0};
static TickType_t s_Bluetooth_LastTick = 0;

static float App_Bluetooth_LimitFloat(float Value, float Min, float Max)
{
    if (Value > Max)
    {
        return Max;
    }
    if (Value < Min)
    {
        return Min;
    }
    return Value;
}

static void App_Bluetooth_SetTarget(float SpeedRpm, float AngularRadps)
{
    SpeedRpm = App_Bluetooth_LimitFloat(SpeedRpm,
                                        -APP_SPEED_TARGET_MAX_RPM,
                                        APP_SPEED_TARGET_MAX_RPM);
    AngularRadps = App_Bluetooth_LimitFloat(AngularRadps,
                                            -APP_ANGULAR_TARGET_MAX_RADPS,
                                            APP_ANGULAR_TARGET_MAX_RADPS);

    portENTER_CRITICAL(&s_Bluetooth_Lock);
    s_Bluetooth_Target.Speed_RPM = SpeedRpm;
    s_Bluetooth_Target.Angular_Radps = AngularRadps;
    s_Bluetooth_LastTick = xTaskGetTickCount();
    portEXIT_CRITICAL(&s_Bluetooth_Lock);
}

static void App_Bluetooth_SetPidValue(const char *Name, float Value)
{
    portENTER_CRITICAL(&s_Bluetooth_Lock);
    if (strcmp(Name, "SpeedKp") == 0)
    {
        s_Bluetooth_PidConfig.SpeedKp = true;
        s_Bluetooth_PidConfig.SpeedKpValue = Value;
    }
    else if (strcmp(Name, "SpeedKi") == 0)
    {
        s_Bluetooth_PidConfig.SpeedKi = true;
        s_Bluetooth_PidConfig.SpeedKiValue = Value;
    }
    else if (strcmp(Name, "SpeedKd") == 0)
    {
        s_Bluetooth_PidConfig.SpeedKd = true;
        s_Bluetooth_PidConfig.SpeedKdValue = Value;
    }
    else if ((strcmp(Name, "TurnKp") == 0) || (strcmp(Name, "AngularKp") == 0))
    {
        s_Bluetooth_PidConfig.AngularKp = true;
        s_Bluetooth_PidConfig.AngularKpValue = Value;
    }
    else if ((strcmp(Name, "TurnKi") == 0) || (strcmp(Name, "AngularKi") == 0))
    {
        s_Bluetooth_PidConfig.AngularKi = true;
        s_Bluetooth_PidConfig.AngularKiValue = Value;
    }
    else if ((strcmp(Name, "TurnKd") == 0) || (strcmp(Name, "AngularKd") == 0))
    {
        s_Bluetooth_PidConfig.AngularKd = true;
        s_Bluetooth_PidConfig.AngularKdValue = Value;
    }
    portEXIT_CRITICAL(&s_Bluetooth_Lock);
}

//获取命令速度目标，服务于运动控制任务
void App_Bluetooth_GetCmdVelTarget(App_Bluetooth_Target_t *Target)
{
    if (Target == NULL)
    {
        return;
    }

    portENTER_CRITICAL(&s_Bluetooth_Lock);
    *Target = s_Bluetooth_Target;
    TickType_t LastTick = s_Bluetooth_LastTick;
    portEXIT_CRITICAL(&s_Bluetooth_Lock);

    if ((LastTick == 0) ||
        ((xTaskGetTickCount() - LastTick) > pdMS_TO_TICKS(APP_BLUETOOTH_CMD_TIMEOUT_MS)))
    {
        Target->Speed_RPM = 0.0f;
        Target->Angular_Radps = 0.0f;
        Target->Is_Timeout = true;
    }
    else
    {
        Target->Is_Timeout = false;
    }
}

//应用PID参数，服务于运动控制任务
void App_Bluetooth_ApplyPidConfig(PID_t *SpeedPid, PID_t *AngularPid)
{
    if ((SpeedPid == NULL) || (AngularPid == NULL))
    {
        return;
    }

    portENTER_CRITICAL(&s_Bluetooth_Lock);
    App_Bluetooth_PidConfig_t PidConfig = s_Bluetooth_PidConfig;
    memset(&s_Bluetooth_PidConfig, 0, sizeof(s_Bluetooth_PidConfig));
    portEXIT_CRITICAL(&s_Bluetooth_Lock);

    if (PidConfig.SpeedKp)
    {
        SpeedPid->kp = PidConfig.SpeedKpValue;
    }
    if (PidConfig.SpeedKi)
    {
        SpeedPid->ki = PidConfig.SpeedKiValue;
    }
    if (PidConfig.SpeedKd)
    {
        SpeedPid->kd = PidConfig.SpeedKdValue;
    }
    if (PidConfig.AngularKp)
    {
        AngularPid->kp = PidConfig.AngularKpValue;
    }
    if (PidConfig.AngularKi)
    {
        AngularPid->ki = PidConfig.AngularKiValue;
    }
    if (PidConfig.AngularKd)
    {
        AngularPid->kd = PidConfig.AngularKdValue;
    }
}
static bool App_Bluetooth_ParseSlider(char *Context)
{
    char *Name = strtok_r(NULL, ",", &Context);
    char *Value = strtok_r(NULL, ",", &Context);

    if ((Name == NULL) || (Value == NULL))
    {
        return false;
    }

    App_Bluetooth_SetPidValue(Name, strtof(Value, NULL));
    return true;
}

static bool App_Bluetooth_ParseJoystick(char *Context)
{
    char *LH_Text = strtok_r(NULL, ",", &Context);//左摇杆水平轴
    char *LV_Text = strtok_r(NULL, ",", &Context);//左摇杆垂直轴
    char *RH_Text = strtok_r(NULL, ",", &Context);//右摇杆水平轴
    char *RV_Text = strtok_r(NULL, ",", &Context);//右摇杆垂直轴

    if ((LH_Text == NULL) || (LV_Text == NULL) ||
        (RH_Text == NULL) || (RV_Text == NULL))
    {
        return false;
    }

    (void)LH_Text;
    (void)RV_Text;

    float LV = strtof(LV_Text, NULL);
    float RH = strtof(RH_Text, NULL);
    float SpeedRpm = LV / 100.0f * APP_SPEED_TARGET_MAX_RPM;
    float AngularRadps = -RH / 100.0f * APP_ANGULAR_TARGET_MAX_RADPS;

    //设置目标速度和角度速度，使用临界区保护
    App_Bluetooth_SetTarget(SpeedRpm, AngularRadps);
    return true;
}

//解析蓝牙数据包
bool App_Bluetooth_ParsePacket(const char *Packet)
{
    if (Packet == NULL)
    {
        return false;
    }

    const char *Head = strchr(Packet, '[');
    const char *Tail = strrchr(Packet, ']');
    if ((Head == NULL) || (Tail == NULL) || (Tail <= Head))
    {
        return false;
    }

    size_t Len = (size_t)(Tail - Head - 1);
    if (Len >= APP_BLUETOOTH_PACKET_MAX_LEN)
    {
        return false;
    }

    char Buffer[APP_BLUETOOTH_PACKET_MAX_LEN];
    memcpy(Buffer, Head + 1, Len);
    Buffer[Len] = '\0';

    char *Context = Buffer;
    char *Tag = strtok_r(Context, ",", &Context);
    if (Tag == NULL)
    {
        return false;
    }

    if (strcmp(Tag, "slider") == 0)
    {
        //解析滑动条数据，用于设置PID参数
        return App_Bluetooth_ParseSlider(Context);
    }
    if (strcmp(Tag, "joystick") == 0)
    {
        //解析摇杆数据，用于设置目标速度和角度速度
        return App_Bluetooth_ParseJoystick(Context);
    }

    return false;
}

//接收蓝牙数据回调函数，解析蓝牙数据
static void App_Bluetooth_RxCallback(const char *Data, uint16_t Len, void *Arg)
{
    (void)Arg;

    if ((Data == NULL) || (Len == 0))
    {
        return;
    }

    for (uint16_t i = 0; i < Len; i++)
    {
        char Ch = Data[i];

        if (Ch == '[')
        {
            s_Bluetooth_RxLen = 0;
        }

        if (s_Bluetooth_RxLen < (sizeof(s_Bluetooth_RxBuffer) - 1))
        {
            s_Bluetooth_RxBuffer[s_Bluetooth_RxLen++] = Ch;
            s_Bluetooth_RxBuffer[s_Bluetooth_RxLen] = '\0';
        }
        else
        {
            s_Bluetooth_RxLen = 0;
            continue;
        }

        if (Ch == ']')
        {//接收到完整的数据，然后调用解析函数
            if (!App_Bluetooth_ParsePacket(s_Bluetooth_RxBuffer))
            {
                ESP_LOGW(TAG, "Invalid packet: %s", s_Bluetooth_RxBuffer);
            }
            s_Bluetooth_RxLen = 0;
        }
    }
}


void App_Bluetooth_SendPidPlot(float SpeedActual,
                               float SpeedTarget,
                               float AngularActual,
                               float AngularTarget)
{
    char Packet[APP_BLUETOOTH_PACKET_MAX_LEN];

    int Len = snprintf(Packet,
                       sizeof(Packet),
                       "[plot,%d,%d,%d,%d]",
                       (int)SpeedActual,
                       (int)SpeedTarget,
                       (int)AngularActual,
                       (int)AngularTarget);

    if ((Len > 0) && (Len < sizeof(Packet)))
    {
        Bluetooth_Send(Packet);
    }
}

void App_Bluetooth_Init_ForAPP(void)
{
    Bluetooth_Init_ForAPP(App_Bluetooth_RxCallback, NULL);
}

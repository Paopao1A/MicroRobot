#ifndef __APP_BLUETOOTH_H__
#define __APP_BLUETOOTH_H__

#include <stdbool.h>

#include "PID.h"

#define APP_BLUETOOTH_PACKET_MAX_LEN    96
#define APP_BLUETOOTH_CMD_TIMEOUT_MS    10000
#define APP_BLUETOOTH_PLOT_PERIOD_MS    100

typedef struct
{
    float Speed_RPM;
    float Angular_Radps;
    bool Is_Timeout;
} App_Bluetooth_Target_t;

void App_Bluetooth_Init_ForAPP(void);
bool App_Bluetooth_ParsePacket(const char *Packet);
void App_Bluetooth_GetCmdVelTarget(App_Bluetooth_Target_t *Target);
void App_Bluetooth_ApplyPidConfig(PID_t *SpeedPid, PID_t *AngularPid);
void App_Bluetooth_SendPacket(const char *Packet);
void App_Bluetooth_SendPidPlot(float SpeedActual,
                               float SpeedTarget,
                               float AngularActual,
                               float AngularTarget);

#endif

#include "LIDAR.h"

#include "BOARD.h"

static ESPLIDAR_Class_t s_legacy_lidar;

static LIDAR_Base_t *LIDAR_Get_Default(void)
{
    if (Esp_LIDAR == NULL)
    {
        ESPLIDAR_Init(&s_legacy_lidar, "ms200_lidar");
        Esp_LIDAR = &s_legacy_lidar.base;
    }

    return Esp_LIDAR;
}

void LIDAR_ReceiveByte(uint8_t data)
{
    LIDAR_RECEIVE_BYTE(LIDAR_Get_Default(), data);
}

bool LIDAR_HasNewScan(void)
{
    return LIDAR_HAS_NEW_SCAN(LIDAR_Get_Default());
}

void LIDAR_ClearNewScan(void)
{
    LIDAR_CLEAR_NEW_SCAN(LIDAR_Get_Default());
}

void LIDAR_GetScan(LIDAR_Scan_t *scan)
{
    LIDAR_GET_SCAN(LIDAR_Get_Default(), scan);
}

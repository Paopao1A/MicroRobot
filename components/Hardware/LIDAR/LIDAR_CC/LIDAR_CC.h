#ifndef __LIDAR_CC_H__
#define __LIDAR_CC_H__

#include <stdbool.h>
#include <stdint.h>

#include "LIDAR_PC.h"

#define MS200_HEAD_1          0xAA  //数据包头1,用于最开始的上电后会辅出一次SN码；
#define MS200_HEAD_2          0x55  //数据包头2
#define MS200_TAIL_1          0x31
#define MS200_TAIL_2          0xF2
#define MS200_FLAG_SN         0x01
#define MS200_FLAG_VERSION    0x02
#define MS200_DATA_START      0x54
#define MS200_BUF_MAX         100
#define MS200_POINT_PER_PACK  12

typedef struct __attribute__((packed))//测量点数据结构体
{
    uint16_t Distance;
    uint8_t Intensity;
} MS200_Point_t;

typedef struct __attribute__((packed))//数据包结构体
{
    uint8_t Header;
    uint8_t Count;
    uint16_t Speed;
    uint16_t Start_Angle;
    MS200_Point_t Points[MS200_POINT_PER_PACK];
    uint16_t End_Angle;
    uint16_t Time_Stamp;
    uint8_t Crc8;
} MS200_Package_t;

typedef struct
{
    LIDAR_Base_t base;
    uint8_t rx_protocol_buf[MS200_BUF_MAX];
    MS200_Package_t package;//当前数据包
    LIDAR_Scan_t scan;
    bool has_new_scan;
} ESPLIDAR_Class_t;

void ESPLIDAR_Init(ESPLIDAR_Class_t *self, const char *name);

#endif

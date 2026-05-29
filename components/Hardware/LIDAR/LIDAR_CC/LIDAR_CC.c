#include "LIDAR_CC.h"

#include <string.h>

#include "esp_err.h"
#include "Hardware_Common.h"

static void ESPLIDAR_ReceiveByte(LIDAR_Base_t *self, uint8_t data);
static bool ESPLIDAR_HasNewScan(LIDAR_Base_t *self);
static void ESPLIDAR_ClearNewScan(LIDAR_Base_t *self);
static void ESPLIDAR_GetScan(LIDAR_Base_t *self, LIDAR_Scan_t *scan);

static const LIDAR_Ops_t ESP_LIDAR_Ops = {
    .LIDAR_RECEIVE_BYTE = ESPLIDAR_ReceiveByte,
    .LIDAR_HAS_NEW_SCAN = ESPLIDAR_HasNewScan,
    .LIDAR_CLEAR_NEW_SCAN = ESPLIDAR_ClearNewScan,
    .LIDAR_GET_SCAN = ESPLIDAR_GetScan,
};

//CRC8校验表
static const uint8_t CRC_TABLE[256] = {
    0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3, 0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c,
    0xa9, 0xe4, 0x33, 0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8, 0xf5,
    0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77, 0x3a, 0x94, 0xd9, 0x0e, 0x43,
    0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55, 0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea,
    0x3e, 0x73, 0xa4, 0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f, 0x62,
    0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff, 0xb2, 0x1c, 0x51, 0x86, 0xcb,
    0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2, 0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d,
    0x88, 0xc5, 0x12, 0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99, 0xd4,
    0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14, 0x59, 0xf7, 0xba, 0x6d, 0x20,
    0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36, 0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89,
    0x63, 0x2e, 0xf9, 0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72, 0x3f,
    0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2, 0xef, 0x41, 0x0c, 0xdb, 0x96,
    0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1, 0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e,
    0xeb, 0xa6, 0x71, 0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa, 0xb7,
    0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35, 0x78, 0xd6, 0x9b, 0x4c, 0x01,
    0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17, 0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8,
};

//计算CRC8校验值
static uint8_t ESPLIDAR_CalcCrc8(const uint8_t *buf, uint8_t len)
{
    uint8_t crc = 0x00;

    for (uint8_t i = 0; i < len; i++)
    {
        crc = CRC_TABLE[(crc ^ buf[i]) & 0xFF];
    }

    return crc;
}

//解析数据包
static esp_err_t ESPLIDAR_ParsePackage(const uint8_t *buf, MS200_Package_t *package)
{
    uint8_t point_count = buf[1] & 0x1F;//多少个测量点数
    uint8_t buf_len = point_count * 3 + 11;//数据包总长

    if ((point_count == 0) || (point_count > MS200_POINT_PER_PACK))
    {
        return ESP_FAIL;
    }

    if (ESPLIDAR_CalcCrc8(buf, buf_len - 1) != buf[buf_len - 1])//校验CRC8校验值
    {
        return ESP_FAIL;
    }

    package->Header = buf[0];//数据包头
    package->Count = point_count;//测量点数
    package->Speed = ((uint16_t)buf[3] << 8) | buf[2];//转速
    package->Start_Angle = ((uint16_t)buf[5] << 8) | buf[4];//起始角度
    package->End_Angle = ((uint16_t)buf[buf_len - 4] << 8) | buf[buf_len - 5];//结束角度
    package->Time_Stamp = ((uint16_t)buf[buf_len - 2] << 8) | buf[buf_len - 3];//时间戳
    package->Crc8 = buf[buf_len - 1];//CRC8校验值

    for (uint8_t i = 0; i < point_count; i++)
    {
        package->Points[i].Distance = ((uint16_t)buf[3 * i + 7] << 8) | buf[3 * i + 6];//每一个测量点的距离
        package->Points[i].Intensity = buf[3 * i + 8];//每一个测量点的强度
    }

    return ESP_OK;
}

//更新扫描数据
static void ESPLIDAR_UpdateScan(ESPLIDAR_Class_t *class)
{
    uint16_t step_angle = 0;

    if (class->package.Count <= 1)
    {
        return;
    }

    if (class->package.End_Angle > class->package.Start_Angle)
    {
        step_angle = (class->package.End_Angle - class->package.Start_Angle) / (class->package.Count - 1);//计算角度间隔
    }
    else//处理结束角度小于起始角度的情况，注意数据包中的角度单位是0.01°
    {
        step_angle = (36000 + class->package.End_Angle - class->package.Start_Angle) / (class->package.Count - 1);//计算角度间隔
    }

    for (uint8_t i = 0; i < class->package.Count; i++)//处理每一个点的数据
    {
        uint16_t angle = ((class->package.Start_Angle + i * step_angle) / 100) % LIDAR_POINT_MAX;//计算真实角度，单位转化为°
        class->scan.Points[angle].Distance_Mm = class->package.Points[i].Distance;//对应角度的距离
        class->scan.Points[angle].Intensity = class->package.Points[i].Intensity;//对应角度的强度
    }

    class->scan.Size = LIDAR_POINT_MAX;
    class->has_new_scan = true;
}

//重置接收缓冲区
static void ESPLIDAR_ResetRx(uint8_t *rx_flag, uint8_t *rx_buf_len, uint8_t *rx_buf_index)
{
    *rx_flag = 0;
    *rx_buf_len = 0;
    *rx_buf_index = 0;
}

//接收字节数据
static void ESPLIDAR_ReceiveByte(LIDAR_Base_t *self, uint8_t data)
{
    ESPLIDAR_Class_t *class = container_of(self, ESPLIDAR_Class_t, base);
    static uint8_t rx_flag = 0;
    static uint8_t rx_buf_len = 0;
    static uint8_t rx_buf_index = 0;

    switch (rx_flag)
    {
    case 0:
        if (data == MS200_HEAD_1)//如果接受到数据包头1
        {
            rx_flag = 1;//进入初始状态1，主要是看雷达上电初始化是否正常
            class->rx_protocol_buf[0] = MS200_HEAD_1;
        }
        else if (data == MS200_DATA_START)//如果接受到数据包头2，表示正常雷达数据来了，开始接收
               {
            rx_flag = 5;
            class->rx_protocol_buf[0] = MS200_DATA_START;
        }
        break;

    case 1:
        rx_flag = (data == MS200_HEAD_2) ? 2 : 0;//如果成功接收到数据包头2，进入状态2
        class->rx_protocol_buf[1] = data;
        break;

    case 2:
        class->rx_protocol_buf[2] = data;
        rx_flag = 3;
        break;

    case 3:
        class->rx_protocol_buf[3] = data;
        rx_flag = 4;
        rx_buf_len = data + 3;
        rx_buf_index = 0;
        break;

    case 4://处理完开机的SN码，开始接收正常数据包
        class->rx_protocol_buf[rx_flag + rx_buf_index] = data;
        rx_buf_index++;
        if (rx_buf_index >= rx_buf_len)
        {
            ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);
            memset(class->rx_protocol_buf, 0, sizeof(class->rx_protocol_buf));
        }
        if ((rx_flag + rx_buf_index) >= MS200_BUF_MAX)
        {
            ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);
            memset(class->rx_protocol_buf, 0, sizeof(class->rx_protocol_buf));
        }
        break;

    case 5:
        class->rx_protocol_buf[1] = data;//低五位表示一个包的测量数量
        rx_flag = 6;
        rx_buf_index = 2;//数据偏移量，从2开始
        rx_buf_len = (data & 0x1F) * 3 + 11;//数据包最长长度
        if (rx_buf_len > MS200_BUF_MAX)
        {
            ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);
        }
        break;

    case 6:
        class->rx_protocol_buf[rx_buf_index] = data;
        rx_buf_index++;
        if (rx_buf_index >= rx_buf_len)//如果数据包接收完成，用接收长度判断是否完成数据包接收，因为没有包尾校验
        {
            ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);//重置接收缓冲区
            if (ESPLIDAR_ParsePackage(class->rx_protocol_buf, &class->package) == ESP_OK)//解析数据包
            {
                ESPLIDAR_UpdateScan(class);
            }
            memset(class->rx_protocol_buf, 0, sizeof(class->rx_protocol_buf));
        }
        if (rx_buf_index >= MS200_BUF_MAX)
        {
            ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);
            memset(class->rx_protocol_buf, 0, sizeof(class->rx_protocol_buf));
        }
        break;

    default:
        ESPLIDAR_ResetRx(&rx_flag, &rx_buf_len, &rx_buf_index);
        break;
    }
}

//检查是否有新的扫描数据
static bool ESPLIDAR_HasNewScan(LIDAR_Base_t *self)
{
    ESPLIDAR_Class_t *class = container_of(self, ESPLIDAR_Class_t, base);
    return class->has_new_scan;
}

//清除新的扫描数据标志
static void ESPLIDAR_ClearNewScan(LIDAR_Base_t *self)
{
    ESPLIDAR_Class_t *class = container_of(self, ESPLIDAR_Class_t, base);
    class->has_new_scan = false;
}

//获取扫描数据,用于任务层调用
static void ESPLIDAR_GetScan(LIDAR_Base_t *self, LIDAR_Scan_t *scan)
{
    ESPLIDAR_Class_t *class = container_of(self, ESPLIDAR_Class_t, base);

    if (scan == NULL)
    {
        return;
    }

    *scan = class->scan;
}

void ESPLIDAR_Init(ESPLIDAR_Class_t *self, const char *name)
{
    self->base.name = name;
    self->base.ops = &ESP_LIDAR_Ops;
    memset(self->rx_protocol_buf, 0, sizeof(self->rx_protocol_buf));
    memset(&self->package, 0, sizeof(self->package));
    memset(&self->scan, 0, sizeof(self->scan));
    self->scan.Size = LIDAR_POINT_MAX;
    self->has_new_scan = false;
}

#ifndef __LIDAR_H__
#define __LIDAR_H__

#include "LIDAR_CC.h"
#include "LIDAR_PC.h"

void LIDAR_ReceiveByte(uint8_t data);
bool LIDAR_HasNewScan(void);
void LIDAR_ClearNewScan(void);
void LIDAR_GetScan(LIDAR_Scan_t *scan);

#endif

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stddef.h>
#include "LED_PC.h"

//定义container_of宏，用于获取结构体成员所在的结构体地址
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

void BOARD_Init(void);
extern LED_Base_t *Esp_LED;//定义LED父类对象，后续所有函数都操作父类

#endif

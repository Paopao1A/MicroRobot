#ifndef __HARDWARE_COMMON_H__
#define __HARDWARE_COMMON_H__

#include <stddef.h>

// 容器宏，用于获取包含结构体的指针
// 例如：struct my_struct *my_struct = container_of(&my_struct_member, struct my_struct, my_member);
// 宏定义：#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#endif


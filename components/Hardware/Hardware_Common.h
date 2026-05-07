#ifndef __HARDWARE_COMMON_H__
#define __HARDWARE_COMMON_H__

#include <stddef.h>

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#endif


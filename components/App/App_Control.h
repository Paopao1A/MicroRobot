#ifndef __APP_CONTROL_H__
#define __APP_CONTROL_H__

#define APP_CONTROL_SOURCE_MICROROS     0   // ROS2控制
#define APP_CONTROL_SOURCE_BLUETOOTH    1   // 蓝牙控制

#define APP_CONTROL_SOURCE              0   //使用蓝牙控制，如使用ROS2控制，注释掉这行

#ifndef APP_CONTROL_SOURCE
#define APP_CONTROL_SOURCE APP_CONTROL_SOURCE_MICROROS
#endif

#endif

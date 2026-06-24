# MicroRobot

基于 ESP32-S3、ESP-IDF/FreeRTOS、micro-ROS 和 ROS2 Humble 的移动机器人控制系统。下位机负责底盘闭环控制、传感器采集、舵机云台控制和 micro-ROS 通信；上位机负责 ROS2 bringup、TF、图像桥接、OpenCV 目标识别和云台视觉跟踪。

## 工作空间

| 内容 | 路径 |
| --- | --- |
| 下位机 ESP-IDF 工程 | `E:\ESP32_Project\MicroRobot` |
| 上位机 ROS2 工作空间 | 虚拟机 `~/MicroRos` |

## 项目目标

- 将移动机器人软件拆分为硬件抽象层、任务层、应用层，降低外设驱动、实时控制和 ROS 通信之间的耦合。
- 使用 micro-ROS 将 ESP32 下位机接入 ROS2 网络，实现底盘速度控制、IMU/里程计/雷达状态回传和云台控制。
- 接入 ESP32-CAM 图像链路，在上位机完成图像桥接、颜色识别和云台跟踪验证。
- 保持“通信回调只更新目标值，任务层固定周期执行控制”的实时控制结构，避免在 ROS 回调中执行重控制逻辑。

## 硬件清单

| 模块 | 说明 |
| --- | --- |
| 主控 | ESP32-S3，ESP-IDF + FreeRTOS + micro-ROS |
| 底盘 | 四路直流电机，四路编码器 |
| 电机驱动 | MCPWM，25 kHz PWM |
| 编码器 | ESP32 PCNT，4 路 AB 相 |
| IMU | ICM42670P，I2C 地址 `0x68` |
| 激光雷达 | MS200 协议解析，UART1，230400 baud |
| 云台 | 2 自由度 pan/tilt 舵机 |
| 舵机 PWM | LEDC，50 Hz，14 bit，避免与电机 MCPWM 资源冲突 |
| 图像 | ESP32-CAM / ROS-WiFi 图传模块，独立 micro-ROS Agent |
| 其他 | LED、蜂鸣器、电池电压 ADC、NimBLE 蓝牙、WiFi |

## 关键引脚与参数

| 功能 | 配置 |
| --- | --- |
| 电机 M1 | GPIO4 / GPIO5 |
| 电机 M2 | GPIO15 / GPIO16 |
| 电机 M3 | GPIO9 / GPIO10 |
| 电机 M4 | GPIO13 / GPIO14 |
| 编码器 H1 | GPIO6 / GPIO7 |
| 编码器 H2 | GPIO47 / GPIO48 |
| 编码器 H3 | GPIO11 / GPIO12 |
| 编码器 H4 | GPIO1 / GPIO2 |
| I2C | SCL GPIO39，SDA GPIO40，400 kHz |
| 雷达 UART | TX GPIO17，RX GPIO18，230400 baud |
| 舵机 PAN | GPIO8，范围 `0~180 deg`，默认 `90 deg` |
| 舵机 TILT | GPIO21，范围 `0~110 deg`，默认 `30 deg` |
| LED | GPIO45 |
| BEEP | GPIO46 |
| 电机 PWM | MCPWM，25 kHz |
| 舵机 PWM | LEDC，50 Hz |
| micro-ROS Agent | `192.168.1.105:8090` |
| micro-ROS Domain ID | `20` |
| ESP32-CAM Agent | `192.168.1.105:9999` |

## 软件架构

```text
main/app_main
    |
    +-- BOARD_Init()     硬件初始化
    |       |
    |       +-- LED / BEEP / ADC / I2C / IMU / LIDAR / ENCODER / PWM / SERVO / TIMER / UART / WIFI
    |
    +-- Task_Init()      FreeRTOS 任务初始化
    |       |
    |       +-- Task_IMU            10 ms IMU 快照
    |       +-- Task_MotionControl  10 ms 底盘控制
    |       +-- Task_Odometry       20 ms 里程计更新
    |       +-- Task_ServoControl   20 ms 云台控制
    |       +-- Task_Lidar          雷达数据读取与解析
    |       +-- Task_BeepControl / Task_LedControl
    |
    +-- App_Init()       micro-ROS / Bluetooth 应用层初始化
            |
            +-- Subscriber: cmd_vel, servo_cmd
            +-- Publisher : odom_raw, imu_raw, scan, servo_state
```

代码入口：

- [main/main.c](main/main.c)
- [components/Hardware/BOARD/BOARD.c](components/Hardware/BOARD/BOARD.c)
- [components/Task/Task_Init/Task_Init.c](components/Task/Task_Init/Task_Init.c)
- [components/App/App_Init/App_Init.c](components/App/App_Init/App_Init.c)

## 分层设计

### 1. 硬件抽象层

目录：`components/Hardware`

硬件层采用 C 语言模拟面向对象风格，核心形式是：

```c
typedef struct Xxx_Base Xxx_Base_t;

typedef struct
{
    void (*OPERATION)(Xxx_Base_t *self, ...);
} Xxx_Ops_t;

struct Xxx_Base
{
    const char *name;
    const Xxx_Ops_t *ops;
};

typedef struct
{
    Xxx_Base_t base;
    // ESP32 平台相关资源句柄
} ESPXxx_Class_t;
```

这种 `Base + Ops + Class` 结构用于 LED、BEEP、ADC、I2C、UART、IMU、LIDAR、ENCODER、PWM、SERVO、WIFI 等模块。上层通过基类指针和接口函数访问硬件，降低平台相关代码对任务层和应用层的影响。

典型文件：

- [components/Hardware/SERVO/SERVO_PC/SERVO_PC.h](components/Hardware/SERVO/SERVO_PC/SERVO_PC.h)
- [components/Hardware/SERVO/SERVO_CC/SERVO_CC.c](components/Hardware/SERVO/SERVO_CC/SERVO_CC.c)
- [components/Hardware/Driver/I2C/I2C_CC/I2C_CC.c](components/Hardware/Driver/I2C/I2C_CC/I2C_CC.c)
- [components/Hardware/IMU/IMU_CC/IMU_CC.c](components/Hardware/IMU/IMU_CC/IMU_CC.c)

### 2. 任务层

目录：`components/Task`

任务层负责实时控制、周期采样和共享数据快照。主要原则是：通信层只写目标值，任务层按固定周期读取目标值并执行控制。

| 任务 | 周期/触发 | 作用 |
| --- | --- | --- |
| `Task_IMU` | 10 ms | 更新 IMU 快照，提供角速度读取接口 |
| `Task_MotionControl` | GPTimer 10 ms 通知 | 读取目标速度，执行速度/角速度 PID，输出四路电机 PWM |
| `Task_Odometry` | 20 ms | 融合编码器线速度和 IMU z 轴角速度，更新 x/y/yaw |
| `Task_ServoControl` | 20 ms | 读取云台目标角度，实际驱动 PAN/TILT 舵机 |
| `Task_Lidar` | 循环读取 UART | 解析雷达扫描数据并缓存 |

典型文件：

- [components/Task/Task_MotionControl/Task_MotionControl.c](components/Task/Task_MotionControl/Task_MotionControl.c)
- [components/Task/Task_Odometry/Task_Odometry.c](components/Task/Task_Odometry/Task_Odometry.c)
- [components/Task/Task_IMU/Task_IMU.c](components/Task/Task_IMU/Task_IMU.c)
- [components/Task/Task_ServoControl/Task_ServoControl.c](components/Task/Task_ServoControl/Task_ServoControl.c)

### 3. 应用层

目录：`components/App`

应用层负责 micro-ROS 节点、publisher、subscriber 和 Bluetooth 控制入口。ROS 控制模式下，`App_Init()` 创建 `App_MicroRosTask`，连接 WiFi 和 micro-ROS Agent 后初始化节点、executor、订阅者和发布者。

典型文件：

- [components/App/App_Init/App_Init.c](components/App/App_Init/App_Init.c)
- [components/App/App_Subscriber_Twist/App_Subscriber_Twist.c](components/App/App_Subscriber_Twist/App_Subscriber_Twist.c)
- [components/App/App_Subscriber_Servo/App_Subscriber_Servo.c](components/App/App_Subscriber_Servo/App_Subscriber_Servo.c)
- [components/App/App_Publisher_Odom/App_Publisher_Odom.c](components/App/App_Publisher_Odom/App_Publisher_Odom.c)
- [components/App/App_Publisher_Imu/App_Publisher_Imu.c](components/App/App_Publisher_Imu/App_Publisher_Imu.c)
- [components/App/App_Publisher_LaserScan/App_Publisher_LaserScan.c](components/App/App_Publisher_LaserScan/App_Publisher_LaserScan.c)
- [components/App/App_Publisher_ServoState/App_Publisher_ServoState.c](components/App/App_Publisher_ServoState/App_Publisher_ServoState.c)

## 运行流程

### 1. 下位机启动

`app_main()` 执行顺序：

```c
void app_main(void)
{
    BOARD_Init();
    Task_Init();
    App_Init();
}
```

对应流程：

1. 初始化板级硬件，构造硬件类对象并赋值给全局基类指针。
2. 创建 IMU、雷达、运动控制、云台控制、里程计等 FreeRTOS 任务。
3. 初始化 WiFi，连接 micro-ROS Agent，创建 ROS2 节点和通信接口。
4. 进入 executor 循环处理 ROS 事件。

### 2. 底盘控制链路

```text
ROS2 /cmd_vel
    |
    v
App_Subscriber_Twist callback
    |
    | 只读取 linear.x / angular.z
    | 只更新共享目标速度
    v
Task_MotionControl 10 ms
    |
    +-- 读取 4 路编码器速度
    +-- 读取 IMU z 轴角速度
    +-- 速度 PID + 角速度 PID
    +-- 差速分配 left_pwm / right_pwm
    v
PWM_Set_Duty(M1..M4)
```

关键代码：

- 目标速度更新：[components/App/App_Subscriber_Twist/App_Subscriber_Twist.c](components/App/App_Subscriber_Twist/App_Subscriber_Twist.c)
- 10 ms 控制任务：[components/Task/Task_MotionControl/Task_MotionControl.c](components/Task/Task_MotionControl/Task_MotionControl.c)
- MCPWM 驱动：[components/Hardware/PWM/PWM_CC/PWM_CC.c](components/Hardware/PWM/PWM_CC/PWM_CC.c)

超时保护：`APP_CMD_VEL_TIMEOUT_MS = 3000`。目标速度超时后清 PID 积分并停止四路电机。

### 3. 里程计与 IMU 链路

```text
Task_IMU 10 ms
    |
    +-- ICM42670P 读取 gyro/accel
    +-- 临界区更新 IMU 快照

Task_Odometry 20 ms
    |
    +-- 读取 4 路编码器计数差
    +-- 计算轮速和线速度
    +-- 读取 IMU gyro z
    +-- 积分 x / y / yaw
    +-- 临界区更新里程计快照

App_Publisher_Odom / App_Publisher_Imu
    |
    +-- 定时发布 ROS2 消息
```

关键代码：

- IMU 快照：[components/Task/Task_IMU/Task_IMU.c](components/Task/Task_IMU/Task_IMU.c)
- 里程计积分：[components/Task/Task_Odometry/Task_Odometry.c](components/Task/Task_Odometry/Task_Odometry.c)
- Odometry 消息：[components/App/App_Publisher_Odom/App_Publisher_Odom.c](components/App/App_Publisher_Odom/App_Publisher_Odom.c)
- IMU 消息：[components/App/App_Publisher_Imu/App_Publisher_Imu.c](components/App/App_Publisher_Imu/App_Publisher_Imu.c)

### 4. 雷达链路

```text
MS200 Lidar
    |
UART1 230400
    |
Task_Lidar
    |
雷达扫描缓存
    |
App_Publisher_LaserScan
    |
ROS2 LaserScan
```

关键代码：

- UART 驱动：[components/Hardware/Driver/UART/UART_CC/UART_CC.c](components/Hardware/Driver/UART/UART_CC/UART_CC.c)
- 雷达协议：[components/Hardware/LIDAR/LIDAR_CC/LIDAR_CC.c](components/Hardware/LIDAR/LIDAR_CC/LIDAR_CC.c)
- 雷达任务：[components/Task/Task_Lidar/Task_Lidar.c](components/Task/Task_Lidar/Task_Lidar.c)
- LaserScan 发布：[components/App/App_Publisher_LaserScan/App_Publisher_LaserScan.c](components/App/App_Publisher_LaserScan/App_Publisher_LaserScan.c)

### 5. 云台控制链路

```text
ROS2 /servo_cmd
    |
    v
App_Subscriber_Servo callback
    |
    | 只做角度限幅并更新目标
    v
Task_ServoControl 20 ms
    |
    +-- 对比目标角度和上次角度
    +-- 调用 SERVO_Set_Angle
    +-- 更新云台状态快照
    v
LEDC 50 Hz PWM -> PAN/TILT
```

关键代码：

- 舵机目标订阅：[components/App/App_Subscriber_Servo/App_Subscriber_Servo.c](components/App/App_Subscriber_Servo/App_Subscriber_Servo.c)
- 舵机任务：[components/Task/Task_ServoControl/Task_ServoControl.c](components/Task/Task_ServoControl/Task_ServoControl.c)
- LEDC 舵机驱动：[components/Hardware/SERVO/SERVO_CC/SERVO_CC.c](components/Hardware/SERVO/SERVO_CC/SERVO_CC.c)
- 舵机状态发布：[components/App/App_Publisher_ServoState/App_Publisher_ServoState.c](components/App/App_Publisher_ServoState/App_Publisher_ServoState.c)

角度约定：

- PAN：`0~180 deg`，默认 `90 deg`
- TILT：`0~110 deg`，默认 `30 deg`
- ROS 消息使用 `geometry_msgs/msg/Vector3`，`x = pan_deg`，`y = tilt_deg`，`z` 暂未使用。

### 6. 上位机视觉链路

上位机工作区：虚拟机 `~/MicroRos`

```text
ESP32-CAM
    |
micro-ROS Agent 9999
    |
/espRos/esp32camera (CompressedImage)
    |
compressed_image_bridge
    |
/camera/image_raw
    |
color_detector
    |
/vision/target + /vision/debug_image
    |
servo_visual_tracker
    |
/servo_cmd
```

当前视觉包：

- `~/MicroRos/src/micro_robot_vision`
- `~/MicroRos/src/my_launch/launch/vision_bringup.launch.py`

注意：

- 底盘主控 Agent 使用 UDP `8090`。
- ESP32-CAM 图传模块使用独立 Agent，UDP `9999`。
- 若 `/espRos/esp32camera` 没有 publisher，优先检查 `~/start_Camera_computer.sh` 是否运行、相机模块 IP 和 Agent 端口是否配置正确。

## ROS2 / micro-ROS 接口

| 方向 | 名称 | 类型 | 周期/说明 |
| --- | --- | --- | --- |
| Subscribe | `cmd_vel` | `geometry_msgs/msg/Twist` | 只使用 `linear.x` 和 `angular.z` |
| Subscribe | `servo_cmd` | `geometry_msgs/msg/Vector3` | `x=pan_deg`，`y=tilt_deg` |
| Publish | `odom_raw` | `nav_msgs/msg/Odometry` | 50 ms |
| Publish | `imu_raw` | `sensor_msgs/msg/Imu` | 20 ms |
| Publish | `scan` | `sensor_msgs/msg/LaserScan` | 90 ms，360 点 |
| Publish | `servo_state` | `geometry_msgs/msg/Vector3` | 100 ms |

micro-ROS 资源配置：

- `RMW_UXRCE_MAX_PUBLISHERS = 5`
- `RMW_UXRCE_MAX_SUBSCRIPTIONS = 4`

如果新增 publisher/subscriber，需要同步检查并重新构建 `libmicroros.a`。

## 共享数据与竞态保护

跨任务共享数据均采用快照或目标值模型：

- 目标速度：订阅回调写入，运动控制任务读取。
- IMU 快照：IMU 任务写入，运动控制、里程计、publisher 读取。
- 里程计快照：里程计任务写入，publisher 读取。
- 云台目标：订阅回调写入，云台任务读取。
- 云台状态：云台任务写入，publisher 读取。

典型保护方式：

```c
static portMUX_TYPE s_Lock = portMUX_INITIALIZER_UNLOCKED;

portENTER_CRITICAL(&s_Lock);
// read or write shared data
portEXIT_CRITICAL(&s_Lock);
```

相关文件：

- [components/App/App_Subscriber_Twist/App_Subscriber_Twist.c](components/App/App_Subscriber_Twist/App_Subscriber_Twist.c)
- [components/App/App_Subscriber_Servo/App_Subscriber_Servo.c](components/App/App_Subscriber_Servo/App_Subscriber_Servo.c)
- [components/Task/Task_IMU/Task_IMU.c](components/Task/Task_IMU/Task_IMU.c)
- [components/Task/Task_Odometry/Task_Odometry.c](components/Task/Task_Odometry/Task_Odometry.c)
- [components/Task/Task_ServoControl/Task_ServoControl.c](components/Task/Task_ServoControl/Task_ServoControl.c)

## 构建与运行

### 下位机

工程路径：

```powershell
cd E:\ESP32_Project\MicroRobot
```

常规 ESP-IDF 构建：

```powershell
idf.py build
idf.py flash monitor
```

本工程当前 `sdkconfig` 中 micro-ROS 关键配置：

```text
CONFIG_MICRO_ROS_AGENT_IP="192.168.1.105"
CONFIG_MICRO_ROS_AGENT_PORT="8090"
CONFIG_MICRO_ROS_DOMAIN_ID=20
CONFIG_MICRO_ROS_APP_STACK=16000
```

### 上位机底盘 Agent

虚拟机中：

```bash
cd ~
./start_agent_computer.sh
```

等价命令：

```bash
docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm \
  --privileged --net=host microros/micro-ros-agent:humble \
  udp4 --port 8090 -v4
```

### 上位机相机 Agent

```bash
cd ~
./start_Camera_computer.sh
```

等价命令：

```bash
docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm \
  --privileged --net=host microros/micro-ros-agent:humble \
  udp4 --port 9999 -v4
```

### 上位机 bringup

```bash
cd ~/MicroRos
source /opt/ros/humble/setup.bash
source install/setup.bash
ros2 launch my_launch base_bringup.launch.py
```

### 上位机视觉

```bash
cd ~/MicroRos
source /opt/ros/humble/setup.bash
source install/setup.bash
ros2 launch my_launch vision_bringup.launch.py
```

## 调试命令

查看 ROS 图：

```bash
ros2 node list
ros2 topic list
```

检查底盘话题：

```bash
ros2 topic echo /odom_raw --once
ros2 topic echo /imu_raw --once
ros2 topic echo /servo_state
ros2 topic hz /scan
```

发布速度：

```bash
ros2 topic pub --once /cmd_vel geometry_msgs/msg/Twist \
  "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"
```

发布云台角度：

```bash
ros2 topic pub --once /servo_cmd geometry_msgs/msg/Vector3 \
  "{x: 90.0, y: 30.0, z: 0.0}"
```

检查相机：

```bash
ros2 topic info -v /espRos/esp32camera
ros2 topic hz /espRos/esp32camera
```

若需要查看 OpenCV 显示窗口，可运行原相机显示节点：

```bash
cd ~/yahboomcar_ws
source /opt/ros/humble/setup.bash
source install/setup.bash
ros2 run yahboom_esp32_camera sub_img
```

## 后续可优化方向

- 提升 ESP32-CAM 图传帧率或降低图像分辨率，改善云台视觉跟踪延迟。
- 将颜色识别和云台跟踪策略进一步参数化，支持运行时调参。
- 对里程计引入更完整的轮速模型和 IMU 姿态融合。
- 接入 SLAM/Nav2，补充 `map -> odom` 坐标关系。
- 增加电池状态 publisher 和系统诊断信息。

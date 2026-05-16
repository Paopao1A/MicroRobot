#include "Task_MotionControl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TIMER.h"
#include "PID.h"
#include "IMU.h"
#include "PWM.h"
#include "Encoder.h"
#include "App_Subscriber.h"

#define TASK_MOTION_CONTROL_STACK_SIZE    4096
#define TASK_MOTION_CONTROL_PRIORITY      8   //运动控制优先级高，保证实时性

static void Task_MotionControl(void *pvParameters);

void Task_MotionControl_Init(void)
{
    TaskHandle_t xTaskHandle = NULL;

    xTaskCreate(Task_MotionControl,
                "Task_MotionControl",
                TASK_MOTION_CONTROL_STACK_SIZE,
                NULL,
                TASK_MOTION_CONTROL_PRIORITY,
                &xTaskHandle);

    if (xTaskHandle != NULL)
    {
        TIMER_SetNotifyTask(xTaskHandle);//设置定时器通知任务句柄
    }
}

static void Task_MotionControl(void *pvParameters)
{
    static float Ave_Speed;//平均速度,r/min

    Speed_PID.target=0.0f;//目标速度，单位为转/分钟，目前先设定确定值，后续是MicroRos订阅的消息
    Angular_PID.target=0.0f;//目标角速度，单位为rad/s，目前先设定确定值，后续是MicroRos订阅的消息
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//等待定时器通知，10ms周期,PDTRUE表示接收到通知后将通知值清零，portMAX_DELAY表示无限等待。
    /*GPTimer 10ms 中断流程
    -> vTaskNotifyGiveFromISR(Task_MotionControl)
    -> Task_MotionControl 通知值 +1
    -> 如果任务正在 ulTaskNotifyTake() 等待，就唤醒它
    -> 如果它优先级更高，请求切换
    -> 中断退出
    -> Task_MotionControl 开始执行一次 10ms 控制逻辑
    */
        //速度和角速度双环PID
        float Ave_Left_Speed=(Encoder_Get_Speed(ENCODER_ID_M1)+Encoder_Get_Speed(ENCODER_ID_M2))/2.0f;//左轮平均速度，单位转/min
        float Ave_Right_Speed=(Encoder_Get_Speed(ENCODER_ID_M3)+Encoder_Get_Speed(ENCODER_ID_M4))/2.0f;//右轮平均速度，单位转/min
        Ave_Speed=(Ave_Left_Speed+Ave_Right_Speed)/2.0f;//整车平均速度

        //订阅速度和角速度目标，在临界区操作，确保线程安全
        App_CmdVel_Target_t CmdVel_Target;
        App_Subscriber_GetCmdVelTarget(&CmdVel_Target);
        Speed_PID.target = CmdVel_Target.Speed_RPM;
        Angular_PID.target = CmdVel_Target.Angular_Radps;
        if (CmdVel_Target.Is_Timeout)//如果上位机心跳包超时，重置PID积分，停止电机
        {
            Speed_PID.error_accumlation = 0.0f;
            Speed_PID.error_last = 0.0f;
            Speed_PID.out = 0.0f;
            Angular_PID.error_accumlation = 0.0f;
            Angular_PID.error_last = 0.0f;
            Angular_PID.out = 0.0f;
            PWM_Stop_Motor(PWM_ID_M1,true);
            PWM_Stop_Motor(PWM_ID_M2,true);
            PWM_Stop_Motor(PWM_ID_M3,true);
            PWM_Stop_Motor(PWM_ID_M4,true);
            continue;
        }

        //速度PID计算
        Speed_PID.actual=Ave_Speed;
        PID_Culculate(&Speed_PID);

        //角速度PID计算
        Angular_PID.actual=IMU_Get_Angular();//获取当前角速度，单位为rad/s
        PID_Culculate(&Angular_PID);

        //根据PID输出计算左右轮PWM占空比，简单差分控制
        float left_pwm = Speed_PID.out - Angular_PID.out;
        float right_pwm = Speed_PID.out + Angular_PID.out;

        //限制PWM占空比在0-100%之间
        if (left_pwm > 200.0f) left_pwm = 200.0f;
        if (left_pwm < -200.0f) left_pwm = -200.0f;

        if (right_pwm > 200.0f) right_pwm = 200.0f;
        if (right_pwm < -200.0f) right_pwm = -200.0f;

        PWM_Set_Duty(PWM_ID_M1, (int)left_pwm);//左前轮
        PWM_Set_Duty(PWM_ID_M2, (int)left_pwm);//左后轮
        PWM_Set_Duty(PWM_ID_M3, (int)right_pwm);//右前轮
        PWM_Set_Duty(PWM_ID_M4, (int)right_pwm);//右后轮

    }

}

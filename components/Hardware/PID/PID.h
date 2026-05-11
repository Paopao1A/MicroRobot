#ifndef __PID_H
#define __PID_H

typedef struct
{
	float target;
	float actual;//当前实际值
	float actual_last;//上一次实际值
	float out;
	
	float kp;
	float ki;
	float kd;
	
	float error_last;//上一次（目标-实际值）的误差
	float error_now;//当前误差
	float error_accumlation;
	
	float outmax;
	float outmin;
	
	float outoffset;//输出补偿值，防止死区或者其他原因导致的输出不足
	float accumlation_max;//积分限幅，防止积分过大导致的系统不稳定
	float accumlation_min;
}PID_t;

void PID_Culculate(PID_t* PID);
extern PID_t Speed_PID;
extern PID_t Angular_PID;

#endif

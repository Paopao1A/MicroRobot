#include "PID.h"

PID_t Speed_PID=
{
	.ki=1.0,
	.kp=0.2,
	.accumlation_max=1000.0f,
	.accumlation_min=-1000.0f,
	.outmax=100.0f,	
	.outmin=-100.0f,
};//速度环PID,暂时积分限幅设置较大，后续调参继续设置

PID_t Angular_PID=
{
	.ki=1.0,
	.kp=0.2,
	.accumlation_max=1000.0f,
	.accumlation_min=-1000.0f,
	.outmax=100.0f,
	.outmin=-100.0f,
};//角度环PID


void PID_Culculate(PID_t* PID)
{
	PID->error_now=PID->target-PID->actual;
	PID->error_accumlation+=PID->error_now;
	
	if(PID->error_accumlation>PID->accumlation_max)
		PID->error_accumlation=PID->accumlation_max;
	else if(PID->error_accumlation<PID->accumlation_min)
		PID->error_accumlation=PID->accumlation_min;
	//PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation+PID->kd*(PID->error_now-PID->error_last);

	//采用微分先行，利用实际值的变化率来预测未来的误差，这样可以让系统响应更快更稳定，特别是对于有较大滞后或者惯性的系统
	PID->out=PID->kp*PID->error_now+PID->ki*PID->error_accumlation-PID->kd*(PID->actual-PID->actual_last);
	
	if(PID->out>PID->outmax)
		PID->out=PID->outmax;
	else if(PID->out<PID->outmin)
		PID->out=PID->outmin;

	PID->error_last=PID->error_now;
	PID->actual_last=PID->actual;
}

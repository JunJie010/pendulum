#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "math.h"

#define EPSILON 0.0001

void PID_Move(PID *p)          //定义PID启动代码
{
	p->Error1 = p->Error0;           //上次误差
	p->Error0 = p->SP - p->PV;       //本次误差
	
	if(fabs(p->Ki)> EPSILON)         //判断Ki非0
	{
		p->ErrorInt += p->Error0;     //误差积分	
	}
	else
		p->ErrorInt = 0;              //防止积分饱和
	
	p->MV = p->Kp * p->Error0 + p->Ki * p->ErrorInt + p->Kd * (p->Error0 - p->Error1);      //位置式PID计算公式
	
	if(p->MV > p->MVmax){p->MV =p->MVmax;}
	else if(p->MV < p->MVmin){p->MV = p->MVmin;}            //输出限幅
	
}

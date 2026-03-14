#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Delay.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "angle.h"
#include "PID.h"

#define Middle_angle 2048           //定义理想中心角度值，注意由于机械设计原因，无法达到理想的中点2048，需要不断调试
#define Middle_range 450            //定义理想中心可调控范围区间

uint8_t Keynum;
uint8_t RunState;          //定义运行状态标志位  1运行 0停止
uint16_t angle;            //定义摆杆角度
int16_t location;         //定义横摆的位置

PID inner={
	.SP=Middle_angle,
	
	.Kp=0.15,
	.Ki=0.002,
	.Kd=0.5,
	
	.MVmax=100,
	.MVmin=-100,          //PWM输出限幅为100
};	

PID outer={
	.SP=0,
	
	.Kp=0.3,
	.Ki=0,
	.Kd=9.0,
	 
	.MVmax=100,        //外环的输出值控制内环的角度,如果太大，横杆移动太快，不利于平衡
	.MVmin=-100,          //在中心角度+-100的范围内变化  
};

int main()
{
	OLED_Init();
	LED_Init();
	Key_Init();
	RP_Init();
	Motor_Init();
	Encoder_Init();
	Serial_Init();
	AD_Init();
	
	Timer_Init();
	
	
	while(1)
	{
		Keynum=Key_GetNum();        //按键扫描
		if(Keynum==1)               
		{
			RunState = !RunState;      //按键1实现启动停止切换   1 0 1 0
		}
		if(Keynum==2)
		{
			outer.SP += 408;             //按键2实现左转一圈
			if(outer.SP > 4080)
			{
				outer.SP = 4080;         //最大转10圈
			}
		}
		if(Keynum==3)
		{
			outer.SP -= 408;             //按键3实现右转一圈
			if(outer.SP < -4080)
			{
				outer.SP = -4080;         //最大转10圈
			}
		}
		
		if(RunState==1)            //编写LED灯显示运行状态
		{
			LED_ON();              
		}
		else 
			LED_OFF();
		
//内环调试	                                           //通过电位器旋钮调节P,I,D三项权重 
//		inner.Kp = RP_GetValue(1)/4095.0 * 1;     //4095后.0防止除的时候丢失小数,(0,4095)映射到(0,1)   
//		inner.Ki = RP_GetValue(2)/4095.0 * 0.1;
//		inner.Kd = RP_GetValue(3)/4095.0 * 1;     

//外环调试
//		outer.Kp = RP_GetValue(1)/4095.0 * 1;     //4095后.0防止除的时候丢失小数,(0,4095)映射到(0,1)   
//		outer.Ki = RP_GetValue(2)/4095.0 * 1;
//		outer.Kd = RP_GetValue(3)/4095.0 * 9; 
		
//		OLED_Printf(0,0,OLED_8X16,"angle=%04d",angle);      //显示当前角度值，用于测试摆杆合适位置
		
		OLED_Printf(0,0,OLED_6X8,"inner");                 //内环显示
		OLED_Printf(0,12,OLED_6X8,"Kp=%05.3f",inner.Kp);
		OLED_Printf(0,20,OLED_6X8,"Ki=%05.3f",inner.Ki);
		OLED_Printf(0,28,OLED_6X8,"Kd=%05.3f",inner.Kd);
		OLED_Printf(0,40,OLED_6X8,"SP=%04.0f",inner.SP);
		OLED_Printf(0,48,OLED_6X8,"PV=%04d",angle);           //此处计划即使不用PID，也能显示摆杆角度
		OLED_Printf(0,56,OLED_6X8,"MV=%+04.0f",inner.MV);
		
		OLED_Printf(64,0,OLED_6X8,"outer");                 //外环显示
		OLED_Printf(64,12,OLED_6X8,"Kp=%05.3f",outer.Kp);
		OLED_Printf(64,20,OLED_6X8,"Ki=%05.3f",outer.Ki);
		OLED_Printf(64,28,OLED_6X8,"Kd=%05.3f",outer.Kd);
		OLED_Printf(64,40,OLED_6X8,"SP=%+05.0f",outer.SP);
		OLED_Printf(64,48,OLED_6X8,"PV=%+05d",location);           //此处计划即使不用PID，也能显示横杆位置
		OLED_Printf(64,56,OLED_6X8,"MV=%+04.0f",outer.MV);
		
		OLED_Update();
		
	}
}

void TIM1_UP_IRQHandler(void)                //编写定时中断函数，1ms自动执行一次
{
	uint16_t count1,count2;                        //定义变量用于PID内外环调控周期的分频         
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)           //定时中断标志位的判断
	{
		Key_Tick();                  //实现非阻塞式按键
		
		angle=AD_GetValue();         //读取摆杆角度     (0,4095)
		location+=Encoder_Get();     //获取横摆位置 
		
		if(!(angle > Middle_angle - Middle_range && angle < Middle_angle + Middle_range))   //若摆杆角度不处于(Middle_angle +- Middle_range)可控范围内
		{
			RunState=0;             //不具备倒立的条件，停止运行
		}
		
		if(RunState==1)              //执行PID程序
		{
			count1++;
			if(count1 >= 2)          //内环调控周期，越快越好，越快响应越灵敏，但其受到传感器频率，执行器极限参数的限制，超过限制没有意义
			{                         //此处调控周期定位6ms
				count1=0;
				
				inner.PV = angle;          //获取角度实际值 
				PID_Move(&inner);          //位置式PID公式  内环
				Motor_SetPWM(-inner.MV);    //根据PID控制器配置PWM精准控制电机转速   注意此处采用反作用控制器
			}
			
			count2++;
			if(count2 >= 36)             //外环的调控要求不严格，不需要太快的速度
			{
				count2=0;
				
				outer.PV = location;       //获取位置实际值
				PID_Move(&outer);          //位置PID公式  外环
				inner.SP = Middle_angle+outer.MV;       //外环的输出为内环的输入(注意看串级控制框图，加上中心角度)
				//若外环输出为0，则内环目标值为中心角度；若不为0，在中心值基础上加减   注意此处采用正作用控制器
			}
		}
		else                         //不执行，电机停转
			Motor_SetPWM(0);          
			
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);           //清除中断标志位，防止中断一直执行
	}
}

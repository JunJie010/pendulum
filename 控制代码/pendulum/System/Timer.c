#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);              //开启Timer1的时钟
	
	TIM_InternalClockConfig(TIM1);                               //选择内部时钟为时基单元的时钟源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;         //选择1时钟分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;      //选择向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                        //选择ARR自动重装器值10000-1
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                     //选择PSC预分频器值7200-1
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                 //选择重复计数器值0，高级定时器才需要用这个
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);                   //初始化时基单元

	TIM_ClearFlag(TIM1, TIM_FLAG_Update);                        //手动清楚 “更新中断”的标志位，防止一上电中断立刻开启，起始值为1
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                       //使能定时器中断，选择更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //选择2位抢占2位响应的优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;             //选择中等密度MD的中断通道TIM1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //中断通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;        //抢占式优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                //响应优先级
	NVIC_Init(&NVIC_InitStructure);                               //初始化NVIC
	
	TIM_Cmd(TIM1, ENABLE);                             //使能定时器1
}

/*
void TIM1_UP_IRQHandler(void)                //编写定时中断函数
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)           //定时中断标志位的判断
	{
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);           //清除中断标志位，防止中断一直执行
	}
}
*/

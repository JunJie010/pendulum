#include "stm32f10x.h"                  // Device header

void Motor_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);    //开启Timer2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //开启GPIOA时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //模式设置为复用推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);                 //初始化GPIOA
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //模式设置为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);                 //初始化GPIOA
	
	TIM_InternalClockConfig(TIM2);                     //选择内部时钟为时基单元的时钟源
	
	TIM_TimeBaseInitTypeDef TIM_TimeBase_Structure;
	TIM_TimeBase_Structure.TIM_ClockDivision=TIM_CKD_DIV1;         //选择1时钟分频
	TIM_TimeBase_Structure.TIM_CounterMode=TIM_CounterMode_Up;      //选择向上计数模式
	TIM_TimeBase_Structure.TIM_Period=100-1;                                //选择ARR自动重装器值100-1
	TIM_TimeBase_Structure.TIM_Prescaler=36-1;                               //选择PSC预分频器值720-1
	TIM_TimeBase_Structure.TIM_RepetitionCounter=0;                       //选择重复计数器值0，高级定时器才需要用这个
	TIM_TimeBaseInit(TIM2,&TIM_TimeBase_Structure);           //初始化时基单元
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);       //没有用完结构体成员，给结构体变量赋初值
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;         //设置为PWM模式1
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;    //极性不翻转
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;    //输出状态使能
	TIM_OCInitStruct.TIM_Pulse=0;                                //设置CCR寄存器值                       
	                                       //带N的成员是高级定时器使用的，这里不需要          
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);                //初始化输出比较单元通道2        
	                                                 //同一个定时器不同通道的频率一样，各自的占空比由各自的CCR决定，相位也是同步的
	
	TIM_Cmd(TIM2,ENABLE);               //使能定时器                       
													//通过ARR,PSC,CCR设置PWM参数为   频率1000，分辨率1%     CRR 0-100
}

void Motor_SetPWM(int8_t PWM)
{
	if(PWM>=0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		TIM_SetCompare1(TIM2,PWM);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		TIM_SetCompare1(TIM2,-PWM);
	}
}

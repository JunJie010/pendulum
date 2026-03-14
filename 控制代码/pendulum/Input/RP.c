#include "stm32f10x.h"                  // Device header

void RP_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);        //RCC开启GPIO和ADC时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                      //配置ADC时钟为6分频    分频后ADC为72MHZ/6=12MHZ
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;      //设置为模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;   
	
 	GPIO_Init(GPIOA, &GPIO_InitStructure);                //初始化GPIO
	
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;             //选择单次转换模式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;        //选择右对齐方式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;     //外部触发方式选择软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;           //配置ADC工作模式为独立工作模式
	ADC_InitStructure.ADC_NbrOfChannel=1;                       //通道数目为1
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;                //选择非扫描模式
	
	ADC_Init(ADC2,&ADC_InitStructure);                  //初始化ADC
	
	ADC_Cmd(ADC2,ENABLE);                         //使能ADC
	
	ADC_ResetCalibration(ADC2);               //复位校准
	while(ADC_GetResetCalibrationStatus(ADC2)==SET);        //等待，返回复位校准状态为0即可下一步,即复位校准完成
	ADC_StartCalibration(ADC2);               //开始校准
	while(ADC_GetCalibrationStatus(ADC2)==SET);            //等待，返回校准状态为0即可下一步，校准完成
                                                   //以上为校准全过程
                                    
}

uint16_t RP_GetValue(uint8_t n)                   //启动转换，调用结果    多通道，设置通道为变量
{
	if(n==1)
	{
		ADC_RegularChannelConfig(ADC2,ADC_Channel_2,1,ADC_SampleTime_55Cycles5);
	}
	if(n==2)
	{
		ADC_RegularChannelConfig(ADC2,ADC_Channel_3,1,ADC_SampleTime_55Cycles5);
	}
	if(n==3)
	{
		ADC_RegularChannelConfig(ADC2,ADC_Channel_4,1,ADC_SampleTime_55Cycles5);
	}
	if(n==4)
	{
		ADC_RegularChannelConfig(ADC2,ADC_Channel_5,1,ADC_SampleTime_55Cycles5);
	}
	       
	ADC_SoftwareStartConvCmd(ADC2,ENABLE);         //软件方式触发转换
	
	while(ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC)==RESET);          //等待，返回ADC状态标志位EOC为1可开始下一步
	
	return ADC_GetConversionValue(ADC2);                  //返回 ADC获取转换值
}


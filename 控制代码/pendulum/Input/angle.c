#include "stm32f10x.h"                  // Device header

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);        //RCC开启GPIO和ADC时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                      //配置ADC时钟为6分频    分频后ADC为72MHZ/6=12MHZ
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;      //设置为模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);                //初始化GPIO
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_55Cycles5);       //配置规则组通道设置，通道0，序列1，转换时间适中
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;             //选择单次转换模式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;        //选择右对齐方式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;     //外部触发方式选择软件触发
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;           //配置ADC工作模式为独立工作模式
	ADC_InitStructure.ADC_NbrOfChannel=1;                       //通道数目为1
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;                //选择非扫描模式
	
	ADC_Init(ADC1,&ADC_InitStructure);                  //初始化ADC
	
	ADC_Cmd(ADC1,ENABLE);                         //使能ADC
	
	ADC_ResetCalibration(ADC1);               //复位校准
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);        //等待，返回复位校准状态为0即可下一步,即复位校准完成
	ADC_StartCalibration(ADC1);               //开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);            //等待，返回校准状态为0即可下一步，校准完成
                                                   //以上为校准全过程
                                    
}

uint16_t AD_GetValue(void)                   //启动转换，调用结果
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);         //软件方式触发转换
	
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);          //等待，返回ADC状态标志位EOC为1可开始下一步
	
	return ADC_GetConversionValue(ADC1);                  //返回 ADC获取转换值
}


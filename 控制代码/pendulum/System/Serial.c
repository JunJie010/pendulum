#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //设置为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;              //设置波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;        //无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx;                //使用发送
	USART_InitStructure.USART_Parity = USART_Parity_No;           //无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;         //停止位设置1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;    //数据位设置8位
	USART_Init(USART1, &USART_InitStructure);                  //初始化USART
	
	USART_Cmd(USART1, ENABLE);                   //使能USART
}

void Serial_SendByte(uint8_t Byte)              //定义串口发送字节函数
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);      //等待，为空可发送数据
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)        //定义串口发送数组函数
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)            //定义串口发送字符串函数
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)         //设置X的Y次方
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)       //定义输出数字的串口函数
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

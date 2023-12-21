#ifndef __USART_H
#define __USART_H
	
	#include "stm32f4xx.h"
	#include "stm32f4xx_conf.h"
	#include <stdio.h>	
	#include "sys.h"
	
	#define USART_REC_LEN  			200  	//定义最大接收字节数 200
	#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	
	extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
	extern u16 USART_RX_STA;         		//接收状态标记
	
	void USART1_Init(uint32_t BaudRate);
	void USART1_Send_String(const char *str);
	int fputc(int ch, FILE *fp);
	void _sys_exit(int x); 
	void USART1_IRQHandler(void);
	
#endif



#ifndef __USART_H
#define __USART_H
	
	#include "stm32f4xx.h"
	#include "stm32f4xx_conf.h"
	#include <stdio.h>	
	#include "sys.h"
	
	#define USART_REC_LEN  			200  	//�����������ֽ��� 200
	#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	
	extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
	extern u16 USART_RX_STA;         		//����״̬���
	
	void USART1_Init(uint32_t BaudRate);
	void USART1_Send_String(const char *str);
	int fputc(int ch, FILE *fp);
	void _sys_exit(int x); 
	void USART1_IRQHandler(void);
	
#endif



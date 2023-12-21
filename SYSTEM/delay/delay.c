#include "delay.h"


void delay_ms(uint32_t nms)
{
	while(nms--)
	{
		SysTick->CTRL = 0; // Disable SysTick���ر�ϵͳ��ʱ��
		SysTick->LOAD = 21000-1; // ��������ʱʱ��
		SysTick->VAL  = 0; // Clear current value as well as count flag����յ�ǰ����ֵ��Ҳ�����COUNTFLAG��־λ
		SysTick->CTRL|= 0x01; // Enable SysTick timer with processor clock��ʹ�ô�����ʱ��168MHz��Ϊʱ��Դ��������ϵͳ��ʱ����ʼ����
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set�����SysTick->CTRL��COUNTFLAG��־λ�Ƿ���1
	
	}
	SysTick->CTRL = 0; // Disable SysTick���ر�ϵͳ��ʱ��
}

//΢�뼶��ʱ

void delay_us(uint32_t nus)
{
	
	
	SysTick->CTRL = 0; // Disable SysTick���ر�ϵͳ��ʱ��
	SysTick->LOAD = nus*21-1; // ��������ʱʱ��
	SysTick->VAL  = 0; // Clear current value as well as count flag����յ�ǰ����ֵ��Ҳ�����COUNTFLAG��־λ
	SysTick->CTRL|= 0x01; // Enable SysTick timer with processor clock��ʹ�ô�����ʱ��168MHz��Ϊʱ��Դ��������ϵͳ��ʱ����ʼ����
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set�����SysTick->CTRL��COUNTFLAG��־λ�Ƿ���1
			
	SysTick->CTRL = 0; // Disable SysTick���ر�ϵͳ��ʱ��
	
}


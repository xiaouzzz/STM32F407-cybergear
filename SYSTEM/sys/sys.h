#ifndef __SYS_H
#define __SYS_H
	#include "stm32f4xx.h" 
	//λ������
	#define PAout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOA_BASE+0x14-0x40000000)*32 + n*4))
	#define PBout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOB_BASE+0x14-0x40000000)*32 + n*4))
	#define PCout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOC_BASE+0x14-0x40000000)*32 + n*4))	
	#define PDout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOD_BASE+0x14-0x40000000)*32 + n*4))
	#define PEout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOE_BASE+0x14-0x40000000)*32 + n*4))
	#define PFout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOF_BASE+0x14-0x40000000)*32 + n*4))
	#define PGout(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOG_BASE+0x14-0x40000000)*32 + n*4))
	

	#define PAin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOA_BASE+0x10-0x40000000)*32 + n*4))
	#define PBin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOB_BASE+0x10-0x40000000)*32 + n*4))
	#define PCin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOC_BASE+0x10-0x40000000)*32 + n*4))	
	#define PDin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOD_BASE+0x10-0x40000000)*32 + n*4))
	#define PEin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOE_BASE+0x10-0x40000000)*32 + n*4))
	#define PFin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOF_BASE+0x10-0x40000000)*32 + n*4))
	#define PGin(n)  (*(volatile uint32_t *)(0x42000000 +(GPIOG_BASE+0x10-0x40000000)*32 + n*4))
	//����Ϊ��ຯ��
	void WFI_SET(void);		//ִ��WFIָ��
	void INTX_DISABLE(void);//�ر������ж�
	void INTX_ENABLE(void);	//���������ж�
	void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
#endif
	

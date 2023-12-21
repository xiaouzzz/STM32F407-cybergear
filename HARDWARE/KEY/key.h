#ifndef __KEY_H
	
#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"

#define KEY0_Pin 	GPIO_Pin_3  	
#define KEY1_Pin 	GPIO_Pin_4

//#define PEout(n)  	(*(uint32_t *)(0x42000000 +(GPIOE_BASE+0x14-0x40000000)*32 + n*4))
//#define PEin(n)   	(*(uint32_t *)(0x42000000 +(GPIOE_BASE+0x10-0x40000000)*32 + n*4))

#define KEY0		PEin(4)
#define KEY1		PEin(3)

void KEY_Init(void);
uint32_t get_key_status(void);


#ifdef __cplusplus
}
#endif

#endif


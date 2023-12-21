#ifndef _DHT11_H_
#define _DHT11_H_

	#include "stm32f4xx.h"
	
	#define PGout(n)  (*(uint32_t *)(0x42000000 +(GPIOG_BASE+0x14-0x40000000)*32 + n*4))
	#define PGin(n)   (*(uint32_t *)(0x42000000 +(GPIOG_BASE+0x10-0x40000000)*32 + n*4))
	#define DQ_w  PGout(9)
	#define DQ_r  PGin(9)
	
	void DHT11_Init(GPIOMode_TypeDef Mode);
	int DHT11_GetInfo(float *RH,float *TH);


	
#endif

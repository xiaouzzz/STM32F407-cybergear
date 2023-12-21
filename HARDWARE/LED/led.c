#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 使能端口F的硬件时钟，就是对端口F供电 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* 配置端口F的9、10号引脚 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//9、10号引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;		//复用功能模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不使能上下拉电阻
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//高速模式，引脚响应更快，但是功耗更高
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14; //13、14号引脚
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	//熄灭所有灯
	PFout(9)  = 1;
	PFout(10) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
}

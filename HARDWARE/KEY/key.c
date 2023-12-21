#include "key.h"

/**
  * @brief  按键初始化
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = KEY0_Pin | KEY1_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}

/**
  * @brief  获取按键状态
  * @param  None
  * @retval 返回按键的状态
  */
uint32_t get_key_status(void)
{
	uint32_t state = 0;
	
	if(KEY0 == 0)	//KEY0按下
		state |= 1<<0;
	if(KEY1 == 0)	//KEY1按下
		state |= 1<<1;
	
	return state;
}



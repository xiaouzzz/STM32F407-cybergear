#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ʹ�ܶ˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* ���ö˿�F��9��10������ */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//9��10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//��ʹ������������
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//����ģʽ��������Ӧ���죬���ǹ��ĸ���
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14; //13��14������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	//Ϩ�����е�
	PFout(9)  = 1;
	PFout(10) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
}

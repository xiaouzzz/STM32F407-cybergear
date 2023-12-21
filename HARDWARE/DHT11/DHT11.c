#include "DHT11.h"
#include "delay.h"

void DHT11_Init(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��PG9-->����ȷ���������ģʽ
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = Mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//�ⲿ��������
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

//��ʪ��-->��ʱ���ź�Ҫ��ߣ�ʱ���ź��������ȡ������ʧ��
int DHT11_GetInfo(float *RH,float *TH)
{
	uint32_t cnt=0;
	uint32_t i,j;
	int RH_int,TH_int;
	float RH_float,TH_float;
	
	char Data[5] = {0};
	//1���������������ź�
	DHT11_Init(GPIO_Mode_OUT);		//����PG9Ϊ���ģʽ
	DQ_w = 0;
	delay_ms(25);	//����18-30����
	DQ_w = 1;
	
	//2���������մӻ����͹���Ӧ���ź�
	DHT11_Init(GPIO_Mode_IN);		//����PG9Ϊ����ģʽ
	
	while(DQ_r==1)	//�ȴ��ӻ�����	--->����83us
	{
		cnt++;
		delay_us(1);	
		if(cnt>=50000)	//��ʱ�ж�
			return -1;
	}
	cnt=0;
	while(DQ_r==0)	//�ȴ��ӻ�����	--->����87us
	{
	
		cnt++;
		delay_us(1);
		if(cnt>=50000)	//��ʱ�ж�
			return -2;
	}
	cnt=0;
	while(DQ_r==1)	//�ȴ��ӻ�����	--->��ʾ��ʼ��������
	{
		cnt++;
		delay_us(1);
		if(cnt>=50000)	//��ʱ�ж�
			return -3;
	}
	
	//3��������ʼ��ȡ�ӻ����ͻ�����40bit��ʪ������
	
	for(i=0;i<5;i++)
	{
		for(j=0;j<8;j++)
		{
			cnt=0;
			while(DQ_r == 0)//�ȴ��͵�ƽ��϶50~58us��ȥ
			{
				delay_us(1);
				cnt++;
				if(cnt>50000)
					return -4;
			}
			delay_us(28);
			if(DQ_r == 1)
			{
				//����1
				Data[i] |= 1<<(7-j);
				cnt=0;
				while(DQ_r == 1)//�ȴ�ʣ�µ�һ��ߵ�ƽʱ���ȥ
				{
					delay_us(1);
					cnt++;
					if(cnt>50000)
						return -5;
				}
			}
		}
	}
	
	if(Data[0]+Data[1]+Data[2]+Data[3]!=Data[4])
		return -4;
	//4�����������߻ص�����״̬
	DQ_w = 1;
	
	
	//5����ȡ����ʪ��ֵת��
	//printf("ʪ��С������%d\r\n�¶�С������%d\r\n",Data[1],Data[3]);
	TH_int 	 = Data[2];
	TH_float = Data[3]*0.1;
	
	RH_int 	 = Data[0];
	RH_float = Data[1]*0.1;
	
	*TH =TH_int+TH_float;
	*RH =RH_int+RH_float;
	
	return 0;
}

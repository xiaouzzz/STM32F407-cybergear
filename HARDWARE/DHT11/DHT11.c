#include "DHT11.h"
#include "delay.h"

void DHT11_Init(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化PG9-->传参确定输入输出模式
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = Mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//外部已有上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

//温湿度-->对时钟信号要求高，时钟信号有问题获取测量会失败
int DHT11_GetInfo(float *RH,float *TH)
{
	uint32_t cnt=0;
	uint32_t i,j;
	int RH_int,TH_int;
	float RH_float,TH_float;
	
	char Data[5] = {0};
	//1、主机发送启动信号
	DHT11_Init(GPIO_Mode_OUT);		//设置PG9为输出模式
	DQ_w = 0;
	delay_ms(25);	//拉低18-30毫秒
	DQ_w = 1;
	
	//2、主机接收从机发送过来应答信号
	DHT11_Init(GPIO_Mode_IN);		//设置PG9为输入模式
	
	while(DQ_r==1)	//等待从机拉低	--->拉低83us
	{
		cnt++;
		delay_us(1);	
		if(cnt>=50000)	//超时判断
			return -1;
	}
	cnt=0;
	while(DQ_r==0)	//等待从机拉高	--->拉高87us
	{
	
		cnt++;
		delay_us(1);
		if(cnt>=50000)	//超时判断
			return -2;
	}
	cnt=0;
	while(DQ_r==1)	//等待从机拉低	--->表示开始传输数据
	{
		cnt++;
		delay_us(1);
		if(cnt>=50000)	//超时判断
			return -3;
	}
	
	//3、主机开始读取从机发送回来的40bit温湿度数据
	
	for(i=0;i<5;i++)
	{
		for(j=0;j<8;j++)
		{
			cnt=0;
			while(DQ_r == 0)//等待低电平间隙50~58us过去
			{
				delay_us(1);
				cnt++;
				if(cnt>50000)
					return -4;
			}
			delay_us(28);
			if(DQ_r == 1)
			{
				//数字1
				Data[i] |= 1<<(7-j);
				cnt=0;
				while(DQ_r == 1)//等待剩下的一点高电平时间过去
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
	//4、将总线拉高回到空闲状态
	DQ_w = 1;
	
	
	//5、获取的温湿度值转换
	//printf("湿度小数部分%d\r\n温度小数部分%d\r\n",Data[1],Data[3]);
	TH_int 	 = Data[2];
	TH_float = Data[3]*0.1;
	
	RH_int 	 = Data[0];
	RH_float = Data[1]*0.1;
	
	*TH =TH_int+TH_float;
	*RH =RH_int+RH_float;
	
	return 0;
}

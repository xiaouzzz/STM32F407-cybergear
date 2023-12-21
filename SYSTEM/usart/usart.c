#include "usart.h"
#include "sys.h"
#include "cybergear.h"
#include "mecanum.h"
uint8_t Serial_TxPacket[5];				//定义发送数据包数组，数据包格式：FF 01 02 03 04 FE
uint8_t Serial_RxPacket[5];				//定义接收数据包数组
uint8_t Serial_RxFlag;					//定义接收数据包标志位

//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#if 0

//串口1初始化
void USART1_Init(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//USART使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//设置引脚复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//串口1过采样
	USART_OverSampling8Cmd(USART1, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate;		//速率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1位停止位
	
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
}
#endif

//发送字符串
void USART1_Send_String(const char *str)
{
	while(*str!='\0')
	{
			USART1->DR = *str;
			//delay_ms(5);
			//while(!(USART1->SR & 0X01<<7));
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);	//等待发送完成
		
			str++;
	}
 
}

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void USART1_Init(uint32_t BaudRate)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1

	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
	//串口1过采样
	USART_OverSampling8Cmd(USART1, ENABLE);
	
	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1

	USART_Cmd(USART1, ENABLE);  //使能串口1 

	USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

#endif
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	static uint8_t RxState = 0;		//定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket = 0;	//定义表示当前接收数据位置的静态变量
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//判断USART1是否接收到了新数据
	{
		uint8_t RxData = USART_ReceiveData(USART1);				//读取数据寄存器，存放在接收的数据变量
		
		/*使用状态机的思路，依次处理数据包的不同部分*/
		
		/*当前状态为0，接收数据包包头*/
		if (RxState == 0)
		{
			if (RxData == 0xFF)			//如果数据确实是包头
			{
				RxState = 1;			//置下一个状态
				pRxPacket = 0;			//数据包的位置归零
			}
		}
		/*当前状态为1，接收数据包数据*/
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;	//将数据存入数据包数组的指定位置
			pRxPacket ++;				//数据包的位置自增
			if (pRxPacket >= 5)			//如果收够4个数据
			{
				RxState = 2;			//置下一个状态
			}
		}
		/*当前状态为2，接收数据包包尾*/
		else if (RxState == 2)
		{
			if (RxData == 0xFE)			//如果数据确实是包尾部
			{
				RxState = 0;			//状态归0
				Serial_RxFlag = 1;		//接收数据包标志位置1，成功接收一个数据包
				uint8_t CMD = Serial_RxPacket[0];
				uint8_t a1 =  (Serial_RxPacket[1]  & 0xF0) >> 4 ;//百位
				uint8_t b1 =  Serial_RxPacket[1]  & 0x0F ;       //十位
				uint8_t c1 =  (Serial_RxPacket[2]  & 0xF0) >> 4 ;//个位
				uint16_t x  =    a1 * 100 + b1 * 10  + c1;
			
			    uint8_t a2 =  Serial_RxPacket[2]  & 0x0F;//百位
				uint8_t b2 =  (Serial_RxPacket[3] & 0xF0 ) >> 4;//十位
				uint8_t c2 =  Serial_RxPacket[3]  & 0x0F;//个位
				uint16_t y  =     a2 * 100 + b2 * 10 + c2;	
	            
				switch(CMD){
					case 0x00:
						CyberGear_Init();
						forward(5);
						DPRINTF("forward\r\n");
					    DPRINTF("Serial_RxPacket[1]:%d\r\n",Serial_RxPacket[1]);
						break;
					case 0x01:
//						CyberGear_Init();
//						Move_Transfrom(Serial_RxPacket[1],Serial_RxPacket[2]);
						DPRINTF("move\r\n");
					    DPRINTF("x:%d\n",x);
					    DPRINTF("y:%d\n",y);
					    
						break;
					case 0x02:
						CyberGear_Init();
						back();
						DPRINTF("brake\r\n");
						break;
					case 0x03:
						CyberGear_Init();
					    left();
						DPRINTF("left turn\r\n");
						break;
					case 0x04:
						CyberGear_Init();
					    right();
						DPRINTF("right CW\r\n");
						break;
					case 0xff:
						stop();
						DPRINTF("stop\r\n");
						DPRINTF("Serial_RxPacket[0]:%d\r\n",Serial_RxPacket[0]);
						break;
					case 0x1f:	//================ 测试连接
						DPRINTF("connect successfully\r\n");
						break;
			
		}		
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//清除标志位
	}
	
}
#endif	


#include "usart.h"
#include "sys.h"
#include "cybergear.h"
#include "mecanum.h"
uint8_t Serial_TxPacket[5];				//���巢�����ݰ����飬���ݰ���ʽ��FF 01 02 03 04 FE
uint8_t Serial_RxPacket[5];				//����������ݰ�����
uint8_t Serial_RxFlag;					//����������ݰ���־λ

//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#if 0

//����1��ʼ��
void USART1_Init(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//USARTʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//�������Ÿ���
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//����1������
	USART_OverSampling8Cmd(USART1, ENABLE);
	
	USART_InitStructure.USART_BaudRate = BaudRate;		//����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1λֹͣλ
	
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
}
#endif

//�����ַ���
void USART1_Send_String(const char *str)
{
	while(*str!='\0')
	{
			USART1->DR = *str;
			//delay_ms(5);
			//while(!(USART1->SR & 0X01<<7));
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);	//�ȴ��������
		
			str++;
	}
 
}

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void USART1_Init(uint32_t BaudRate)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��

	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1

	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
	
	//����1������
	USART_OverSampling8Cmd(USART1, ENABLE);
	
	//USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1

	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 

	USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

#endif
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	static uint8_t RxState = 0;		//�����ʾ��ǰ״̬��״̬�ľ�̬����
	static uint8_t pRxPacket = 0;	//�����ʾ��ǰ��������λ�õľ�̬����
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//�ж�USART1�Ƿ���յ���������
	{
		uint8_t RxData = USART_ReceiveData(USART1);				//��ȡ���ݼĴ���������ڽ��յ����ݱ���
		
		/*ʹ��״̬����˼·�����δ������ݰ��Ĳ�ͬ����*/
		
		/*��ǰ״̬Ϊ0���������ݰ���ͷ*/
		if (RxState == 0)
		{
			if (RxData == 0xFF)			//�������ȷʵ�ǰ�ͷ
			{
				RxState = 1;			//����һ��״̬
				pRxPacket = 0;			//���ݰ���λ�ù���
			}
		}
		/*��ǰ״̬Ϊ1���������ݰ�����*/
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;	//�����ݴ������ݰ������ָ��λ��
			pRxPacket ++;				//���ݰ���λ������
			if (pRxPacket >= 5)			//����չ�4������
			{
				RxState = 2;			//����һ��״̬
			}
		}
		/*��ǰ״̬Ϊ2���������ݰ���β*/
		else if (RxState == 2)
		{
			if (RxData == 0xFE)			//�������ȷʵ�ǰ�β��
			{
				RxState = 0;			//״̬��0
				Serial_RxFlag = 1;		//�������ݰ���־λ��1���ɹ�����һ�����ݰ�
				uint8_t CMD = Serial_RxPacket[0];
				uint8_t a1 =  (Serial_RxPacket[1]  & 0xF0) >> 4 ;//��λ
				uint8_t b1 =  Serial_RxPacket[1]  & 0x0F ;       //ʮλ
				uint8_t c1 =  (Serial_RxPacket[2]  & 0xF0) >> 4 ;//��λ
				uint16_t x  =    a1 * 100 + b1 * 10  + c1;
			
			    uint8_t a2 =  Serial_RxPacket[2]  & 0x0F;//��λ
				uint8_t b2 =  (Serial_RxPacket[3] & 0xF0 ) >> 4;//ʮλ
				uint8_t c2 =  Serial_RxPacket[3]  & 0x0F;//��λ
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
					case 0x1f:	//================ ��������
						DPRINTF("connect successfully\r\n");
						break;
			
		}		
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//�����־λ
	}
	
}
#endif	


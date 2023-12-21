#include "can.h"


CanTxMsg CAN1_TxMessage;
CanRxMsg CAN1_RxMessage;

/**
  * @brief  CAN1初始化函数
  * @param  None
  * @retval None
  */
void CAN1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef  CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//CAN引脚配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Connect CAN pins to PA11、PA12 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); 
	
	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//高速模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			//无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//CAN配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	//恢复未初始化状态
	CAN_DeInit(CAN1);
	
	//CAN总线单元初始化
	CAN_InitStructure.CAN_TTCM = DISABLE;	//时间触发
	CAN_InitStructure.CAN_ABOM = DISABLE;	//自动离线
	CAN_InitStructure.CAN_AWUM = DISABLE;	//自动唤醒
	CAN_InitStructure.CAN_NART = DISABLE;	//自动重发
	CAN_InitStructure.CAN_RFLM = DISABLE;	//锁定接收
	CAN_InitStructure.CAN_TXFP = DISABLE;	//发送报文优先级
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//CAN_Mode_Normal;//正常模式
	//配置CAN波特率 42M /4/(1+12+8) = 500Kbps  	CAN波特率=APB总线频率/分频系数/(TBS1+TBS2+SJY)
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	//再同步补偿宽度极值
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;	//相位缓冲段1
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;	//相位缓冲段2
	CAN_InitStructure.CAN_Prescaler = 4;		//4分频
	CAN_Init(CAN1, &CAN_InitStructure);
	
	//CAN过滤器初始化
	CAN_FilterInitStructure.CAN_FilterNumber = 0;			//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;	//过滤器模式 掩码
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//32位掩码长度
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//安装到过滤器FIFO0
	//全部通过
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;		//过滤器ID寄存器高16位
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;		//过滤器ID寄存器低16位
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;	//过滤器ID掩码寄存器高16位
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;	//过滤器ID掩码寄存器低16位
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	//使能过滤
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	//初始化CAN接收中断
	NVIC_InitStruct.NVIC_IRQChannel = CAN1_RX0_IRQn;		//CAN1 FIFO0接收中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x2;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x2;		//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能
	NVIC_Init(&NVIC_InitStruct);
	
	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  CAN1_SendData
  * @param  data：发送的数据
  * @param  len：发送的数据长度
  * @param  msgId：消息ID
  * @retval 成功返回0, 失败返回-1
  */
int8_t CAN1_SendData(uint8_t data[], uint8_t len, uint32_t msgId)
{
	uint8_t mailbox;
	int cnt = 100;
	if(len>8)
	{
		return -1;
	}
	//IDE标志区分标准帧、扩展帧
	if(msgId>0x7FF)
	{
		CAN1_TxMessage.IDE = CAN_Id_Extended;
	}else{
		CAN1_TxMessage.IDE = CAN_Id_Standard;
	}
	
	//标志ID的值 0 - 0x7FF
	//扩展ID的值 0 - 0x1FFFFFFF
	CAN1_TxMessage.StdId = msgId;	//标准帧ID
	CAN1_TxMessage.ExtId = 0xBB;	//扩展帧ID
	CAN1_TxMessage.RTR = CAN_RTR_Data;	//数据帧、遥控帧
	CAN1_TxMessage.DLC = len;			//数据段长度
	//memset(CAN1_TxMessage.Data, 0, 8);	//发送数据
	
	for(int i= 0; i<len; i++)
	{
		CAN1_TxMessage.Data[i] = data[i];
	}
	
	//发送数据
	mailbox = CAN_Transmit(CAN1, &CAN1_TxMessage);
	//printf("mailbox = %d\n", mailbox);
	do{
		cnt--;
		delay_ms(50);
	}while(CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok && cnt);
	
	if(cnt<=0)
		return -1;
	else
		return 0;
}

/**
  * @brief  CAN1接收中断
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	//收到数据
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_RxMessage);
		//中断处理操作
		#if 1
		printf("Recv:");
		for(int i = 0; i<8; i++)
		{
			printf("%02X", CAN1_RxMessage.Data[i]);
		}
		printf("\n");
		#else
		//点灯
		
		
		#endif
		//清除中断标志位
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}



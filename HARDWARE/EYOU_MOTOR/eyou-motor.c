#include "eyou-motor.h"

CanTxMsg Tx_Eyou_Message; 
CanRxMsg Rx_Eyou_Message;
uint8_t motor_id = 0;

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
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //正常模式 CAN_Mode_Normal
	//配置CAN波特率
	#if 1
	// 总体配置保持tBS1>=tBS2 tBS2>=1个CAN时钟周期  tBS2>=2tSJW
	// 42M /4/(1+12+8) = 500Kbps  	CAN波特率 = APB总线频率/分频系数/(TBS1+TBS2+SJY)
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	  //再同步补偿宽度极值
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;	  //相位缓冲段1
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;	  //相位缓冲段2
	CAN_InitStructure.CAN_Prescaler = 4;		  //4分频
	CAN_Init(CAN1, &CAN_InitStructure);
	#else
	// 42M /2/(1+12+8) = 1000Kbps = 1Mbps	CAN波特率 = APB总线频率/分频系数/(TBS1+TBS2+SJY)
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	  //再同步补偿宽度极值
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;	  //相位缓冲段1
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;	  //相位缓冲段2
	CAN_InitStructure.CAN_Prescaler = 2;		  //2分频
	CAN_Init(CAN1, &CAN_InitStructure);
	
	#endif
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
  * @brief  给意优电机发送数据
  * @param  msg 需发送的数据信息
  * @retval 成功返回0,失败返回-1
  */
int8_t Send_Data_to_eyou_Motor(Eyou_Motor_Data_t *msg)
{
	int cnt = 100;
	uint8_t mailbox;
	CanTxMsg Tx_Eyou_Message;
	Tx_Eyou_Message.DLC = msg->dataLen;
	Tx_Eyou_Message.RTR = CAN_RTR_Data;
	Tx_Eyou_Message.IDE = CAN_ID_STD;
	Tx_Eyou_Message.StdId   = msg->motor_id;
	Tx_Eyou_Message.Data[0] = msg->cmd;
	Tx_Eyou_Message.Data[1] = msg->addr;
	
	for(int i=0,j=2; i<msg->dataLen; i++)
	{
		Tx_Eyou_Message.Data[j] = msg->data[i];
	}
	
	mailbox = CAN_Transmit(CAN1, &Tx_Eyou_Message);
	do{
		cnt--;
		delay_ms(50);
	}while(CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok && cnt);
	if(cnt<=-1)
		return -1;
	else
		return 0;
}

/**
  * @brief  获取设备CAN ID
  * @param  void
  * @retval 电机的CAN ID
  */
uint8_t Get_Eyou_Motor_Id(void)
{
	uint8_t motor_can_id = 0;
	Eyou_Motor_Data_t msg;
	memset(&msg, 0, sizeof(msg));
	
	msg.motor_id = 0;
	msg.dataLen  = 6;
	msg.cmd  = READ_CMD;			//读取命令
	msg.addr = DEVICE_SERIAL_NUM;//设备序列号
	
	Send_Data_to_eyou_Motor(&msg);
	//发送完后，在设备的中断中获取接收的数据，提取出设备的CAN ID
	
	
	return motor_can_id;
}

/**
  * @brief  写入数据到意优电机
  * @param  void
  * @retval 成功返回0,失败返回-1
  */
int8_t Write_Data_to_Eyou_Motor(uint8_t data[])
{
	Eyou_Motor_Data_t msg;
	msg.motor_id = 0x01;
	msg.dataLen = 6;
	msg.data[0] = data[0];
	return Send_Data_to_eyou_Motor(&msg);
}

/**
  * @brief  读取意优电机数据
  * @param  void
  * @retval 电机的CAN ID
  */
void Read_Data_from_Eyou_Motor()
{
	
	
}



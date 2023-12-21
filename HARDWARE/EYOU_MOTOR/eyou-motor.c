#include "eyou-motor.h"

CanTxMsg Tx_Eyou_Message; 
CanRxMsg Rx_Eyou_Message;
uint8_t motor_id = 0;

/**
  * @brief  CAN1��ʼ������
  * @param  None
  * @retval None
  */
void CAN1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef  CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//CAN��������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Connect CAN pins to PA11��PA12 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); 
	
	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//CAN����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	//�ָ�δ��ʼ��״̬
	CAN_DeInit(CAN1);
	
	//CAN���ߵ�Ԫ��ʼ��
	CAN_InitStructure.CAN_TTCM = DISABLE;	//ʱ�䴥��
	CAN_InitStructure.CAN_ABOM = DISABLE;	//�Զ�����
	CAN_InitStructure.CAN_AWUM = DISABLE;	//�Զ�����
	CAN_InitStructure.CAN_NART = DISABLE;	//�Զ��ط�
	CAN_InitStructure.CAN_RFLM = DISABLE;	//��������
	CAN_InitStructure.CAN_TXFP = DISABLE;	//���ͱ������ȼ�
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //����ģʽ CAN_Mode_Normal
	//����CAN������
	#if 1
	// �������ñ���tBS1>=tBS2 tBS2>=1��CANʱ������  tBS2>=2tSJW
	// 42M /4/(1+12+8) = 500Kbps  	CAN������ = APB����Ƶ��/��Ƶϵ��/(TBS1+TBS2+SJY)
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	  //��ͬ��������ȼ�ֵ
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;	  //��λ�����1
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;	  //��λ�����2
	CAN_InitStructure.CAN_Prescaler = 4;		  //4��Ƶ
	CAN_Init(CAN1, &CAN_InitStructure);
	#else
	// 42M /2/(1+12+8) = 1000Kbps = 1Mbps	CAN������ = APB����Ƶ��/��Ƶϵ��/(TBS1+TBS2+SJY)
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	  //��ͬ��������ȼ�ֵ
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;	  //��λ�����1
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;	  //��λ�����2
	CAN_InitStructure.CAN_Prescaler = 2;		  //2��Ƶ
	CAN_Init(CAN1, &CAN_InitStructure);
	
	#endif
	//CAN��������ʼ��
	CAN_FilterInitStructure.CAN_FilterNumber = 0;			//������0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;	//������ģʽ ����
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//32λ���볤��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//��װ��������FIFO0
	//ȫ��ͨ��
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;		//������ID�Ĵ�����16λ
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;		//������ID�Ĵ�����16λ
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;	//������ID����Ĵ�����16λ
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;	//������ID����Ĵ�����16λ
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;	//ʹ�ܹ���
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	//��ʼ��CAN�����ж�
	NVIC_InitStruct.NVIC_IRQChannel = CAN1_RX0_IRQn;		//CAN1 FIFO0�����ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x2;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x2;		//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ��
	NVIC_Init(&NVIC_InitStruct);
	
	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  �����ŵ����������
  * @param  msg �跢�͵�������Ϣ
  * @retval �ɹ�����0,ʧ�ܷ���-1
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
  * @brief  ��ȡ�豸CAN ID
  * @param  void
  * @retval �����CAN ID
  */
uint8_t Get_Eyou_Motor_Id(void)
{
	uint8_t motor_can_id = 0;
	Eyou_Motor_Data_t msg;
	memset(&msg, 0, sizeof(msg));
	
	msg.motor_id = 0;
	msg.dataLen  = 6;
	msg.cmd  = READ_CMD;			//��ȡ����
	msg.addr = DEVICE_SERIAL_NUM;//�豸���к�
	
	Send_Data_to_eyou_Motor(&msg);
	//����������豸���ж��л�ȡ���յ����ݣ���ȡ���豸��CAN ID
	
	
	return motor_can_id;
}

/**
  * @brief  д�����ݵ����ŵ��
  * @param  void
  * @retval �ɹ�����0,ʧ�ܷ���-1
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
  * @brief  ��ȡ���ŵ������
  * @param  void
  * @retval �����CAN ID
  */
void Read_Data_from_Eyou_Motor()
{
	
	
}



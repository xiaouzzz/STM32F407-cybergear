#include "cybergear.h"

/*
С��CyberGear΢�������

���ͨ��Ϊ CAN 2.0 ͨ�Žӿڣ������� 1Mbps������ ��չ֡��ʽ

���֧�ֵĿ���ģʽ������
�˿�ģʽ����������˿� 5 ��������
����ģʽ���������ָ���� Iq ������
�ٶ�ģʽ���������ָ���������ٶȣ�
λ��ģʽ���������ָ����λ�ã���������е���ָ����λ��

������				29 λ ID						8Byte������
��С 	Bit28~bit24   	 bit23~8    bit7~0 			Byte0~Byte7
���� 	ͨ������ 	 	 ������ 2   Ŀ���ַ 		������ 1

С��΢���ͨ�����ݾ��� 29λID + 8�ֽ� ���ݵĸ�ʽ

*/

CanTxMsg CAN1_TxMessage;
CanRxMsg CAN1_RxMessage;
uint8_t master_id = 100;
uint8_t motor_id[4] = {1,2,3,4};
uint8_t mcu_id[8] = {0};
uint8_t motor_state;	
CyberGear_Info_t recv_data;	//���յ���������ݽṹ��
CyberGear_FeedBack_t motor_info;

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
	#if 0
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
  * @brief  floatתint�����ݴ����
  * @param  f_val		float��ֵ
  * @param  f_val_min	float��ֵ����Сֵ
  * @param  f_val_max	float��ֵ�����ֵ
  * @param  int_bits	int������λ��
  * @retval float����תΪint��ֵ
  */
int float_to_uint(float f_val, float f_val_min, float f_val_max, int int_bits)
{
	float span = f_val_max - f_val_min; 
	float offset = f_val_min; 
	
	if(f_val > f_val_max)
	{
		f_val = f_val_max; 
	}else if(f_val < f_val_min)
	{
		f_val = f_val_min; 
	}
	
	return (int) ((f_val-offset)*((float)((1<<int_bits)-1))/span);
}

/**
  * @brief  С��΢�����ʼ��
  * @param  None
  * @retval None
  */
void CyberGear_Init(void)
{
	uint16_t can_id;
	uint8_t mcu_id[8];
	DPRINTF("CyberGear Initing...\r\n");
	//CANͨ�ų�ʼ��
	CAN1_Init();
	//��ȡ�����Ϣ
	CyberGear_Id_get(&can_id, mcu_id);	
	CyberGear_Eanble_Run(motor_id[0], master_id);
	CyberGear_Eanble_Run(motor_id[1], master_id);
	CyberGear_Eanble_Run(motor_id[2], master_id);
	CyberGear_Eanble_Run(motor_id[3], master_id);
	//���ͳ�ʼ������Ϣ
}

/**
  * @brief  �������ݵ�С��΢���
  * @param  data ��Ҫ���͵�����
  * @retval �ɹ�����0��ʧ�ܷ���-1
  */
int Send_Data_to_CyberGear(CyberGear_Info_t *data)
{
	uint8_t mailbox;
	CanTxMsg CAN1_TxMessage;
	int cnt = 100;
	
	CAN1_TxMessage.DLC = 8;
	CAN1_TxMessage.IDE = CAN_ID_EXT;
	CAN1_TxMessage.RTR = CAN_RTR_DATA;
	CAN1_TxMessage.ExtId = *((uint32_t *)(&(data->id)));
	
	for(int i=0; i<8; i++)
	{
		CAN1_TxMessage.Data[i] = data->data[i];
	}
	
	mailbox = CAN_Transmit(CAN1, &CAN1_TxMessage);
	do{
		cnt--;
		delay_ms(50);
	}while(CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok && cnt);
	delay_ms(1);
	if(cnt<=0)
		return -1;
	else
		return 0;
}


/**
  * @brief  ��ȡ�豸 ID ��ͨ������ 0�� ����ȡ�豸�� ID �� 64 λ MCU Ψһ��ʶ��
  * @param  can_id	���CAN ID
  * @param 	mcu_id	���MCU ID
  * @retval None
  */
void CyberGear_Id_get(uint16_t *can_id, uint8_t *mcu_id)
{
	DPRINTF("FUNCTION:%s\tLINE:%d\r\n", __FUNCTION__, __LINE__);
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = GET_MOTOR_ID;
	msg.id.dst_id = 0;
	msg.id.data = 0;
	msg.id.res = 0;
	
	for(int i=0; i<8; i++)
	{
		msg.data[i] = 0;
	}
	//��������
	int ret = Send_Data_to_CyberGear(&msg);
	DPRINTF("ret=%d\r\n",ret);
	//����Ӧ��
	/*
		Ӧ��֡��	 ��������������������������������������������������������������������������������������������������������������������������������������������
		������ 		|			   29 λ ID 					    |	8Byte ������   	   |
		��С 	 	| Bit28~bit24 |  bit23~8 bit7~0   |	Byte0~Byte7	|				   	   |
		���� 		|     0 	  |	 Ŀ���� CAN_ID  | 0XFE   		|  64 λMCU Ψһ��ʶ�� |
					|______________________________________________________________________|
	*/
	DPRINTF("FUNCTION:%s\tLINE:%d\r\n", __FUNCTION__, __LINE__);

}

/**
  * @brief  �޸�С��΢��� CANͨ��ID (ģʽ7)
  * @param  old_id	�����ǰCAN ID
  * @param 	new_id	����µ�CAN ID
  * @param 	master_id	��ʶ����ID
  * @retval None
  */
void Change_CyberGear_CAN_Id(uint8_t old_id, uint8_t new_id, uint8_t master_id)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = SET_MOTOR_ID;	//ģʽ7
	msg.id.dst_id = old_id;
	msg.id.data = new_id << 8 | master_id;	//id.data��bit8 ~ bit23
	msg.id.res = 0;
	
	for(int i=0; i<8; i++)
	{
		msg.data[i] = 0;
	}
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief	�������ģʽ
  * @param  motor_id �����ǰCAN ID
  * @param  torque 	 ���Ť��
  * @param 	MechPosition Ŀ��Ƕ�
  * @param 	speed	 ���ٶ�
  * @param 	kp		 ��Ӧ�ٶ�(����λ�ÿ���)��һ��ȡ1-10
  * @param 	kd		 ������ᣬ��С���𵴣��������������ԡ�һ��ȡ0.5����
  * @retval None
  */
void CyberGear_Control_Mode(uint8_t motor_id, float torque, float MechPosition, float speed, float kp, float kd)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = MOTOR_MOVE_CNL;
	msg.id.dst_id = motor_id;	//Ŀ����ID
	msg.id.data = float_to_uint(torque, T_MIN, T_MAX, 16);		   //���� (0~65535)
	msg.id.res = 0;
	
	msg.data[0]=float_to_uint(MechPosition, P_MIN, P_MAX, 16)>>8;  //Byte0~1: Ŀ��Ƕ�[0~65535],  ��Ӧ(-4��~4��)
	msg.data[1]=float_to_uint(MechPosition, P_MIN, P_MAX, 16); 	
	msg.data[2]=float_to_uint(speed, V_MIN, V_MAX, 16)>>8;		   //Byte2~3: Ŀ����ٶ�[0~65535],��Ӧ(-30rad/s~30rad/s)
	msg.data[3]=float_to_uint(speed, V_MIN, V_MAX, 16); 
	msg.data[4]=float_to_uint(kp, KP_MIN, KP_MAX, 16)>>8; 		   //Byte4~5��Kp [0~65535]��Ӧ (0.0~500.0)
	msg.data[5]=float_to_uint(kp, KP_MIN, KP_MAX, 16); 
	msg.data[6]=float_to_uint(kd, KD_MIN, KD_MAX, 16)>>8; 		   //Byte6~7��Kd [0~65535]��Ӧ (0.0~5.0)
	msg.data[7]=float_to_uint(kd, KD_MIN, KD_MAX, 16);
	
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief	�޸ĵ��Ť��
  * @param  motor_id �����ǰCAN ID
  * @param  torque 	 ���Ť��
  * @retval None
  */
void CyberGear_Change_Torque(uint8_t motor_id, float torque)
{
	CyberGear_Control_Mode(motor_id, torque, 0.0, 0.0, 0.0, 0.0);
}

/**
  * @brief	�����ת��ĳһλ��
  * @param  motor_id �����ǰCAN ID
  * @param 	MechPosition Ŀ��Ƕ�
  * @param 	kp		 ��Ӧ�ٶ�
  * @param 	kd		 �������
  * @retval None
  */
void CyberGear_Turn_Certain_Position(uint8_t motor_id, float MechPosition, float kp, float kd)
{
	CyberGear_Control_Mode(motor_id, 0.0, MechPosition, 0.0, kp, kd);
	//CyberGear_Control_Mode(motor_id, 0.0,  3.14, 0.0, 0.5, 0.5);	//��ת1Ȧ
	//CyberGear_Control_Mode(motor_id, 0.0, -3.14, 0.0, 0.5, 0.5);	//��ת1Ȧ
}

/**
  * @brief	�������ģʽ
  * @param  motor_id �����ǰCAN ID
  * @param 	speed	 ���ٶ�(speed>0��ת,speed<0��ת)
  * @param 	kd		 �������
  * @retval None
  */
void CyberGear_Change_Speed(uint8_t motor_id, float speed, float kd)
{
	CyberGear_Control_Mode(motor_id, 0.0, 0.0, speed, 0.0, kd);
}

/**
  * @brief  ���ʹ������
  * @param  motor_id	���CAN ID
  * @param 	master_id	��ʶ���� CAN ID
  * @retval None
  */
void CyberGear_Eanble_Run(uint8_t motor_id, uint8_t master_id)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	
	msg.id.mode = MOTOR_RUN; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF; 
	msg.id.res = 0; 
	
	for(int i=0; i<8; i++)
	{
		msg.data[i] = 0;
	}
	
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief  ���ֹͣ����
  * @param  motor_id	���CAN ID
  * @param 	master_id	��ʶ����CAN ID
  * @retval None
  */
void CyberGear_Stop_Run(uint8_t motor_id, uint8_t master_id)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = MOTOR_STOP; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF; 
	msg.id.res = 0; 
	
	for(int i=0; i<8; i++)
	{
		msg.data[i] = 0;
	}
	
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief  ���õ����е��λ
  * @param  motor_id	���CAN ID
  * @param 	master_id	��ʶ����CAN ID
  * @retval None
  */
void Set_CyberGear_Zero_Position(uint8_t motor_id, uint8_t master_id)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = MOTOR_ZERO_POS; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id; 
	msg.id.res = 0; 
	
	msg.data[0] = 1;	//Byte[0] = 1
	for(int i=1; i<8; i++)
	{
		msg.data[i] = 0;
	}
	
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief  ��ȡ��������
  * @param  motor_id	���CAN ID(�ӻ�)
  * @param 	master_id	����CAN ID
  * @param 	index	������
  * @retval None
  */
void Read_CyberGear_Single_Param(uint8_t motor_id, uint8_t master_id, uint16_t index)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = READ_ONE_PARAM; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF;
	msg.id.res = 0; 
	
	msg.data[0] = index & 0xFF;	//Byte[0]��Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	for(int i=2; i<8; i++)
	{
		msg.data[i] = 00;
	}
	//��������
	Send_Data_to_CyberGear(&msg);
	
	//���մ���������
	
}

#if 0
/**
  * @brief  ��������д��
  * @param  motor_id	���CAN ID(�ӻ�)
  * @param 	master_id	����CAN ID
  * @param 	index	������
  * @retval None
  */
void Write_CyberGear_Single_Param(uint8_t motor_id, uint8_t master_id, uint16_t index)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = WRITE_ONE_PARAM; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF; 
	msg.id.res = 0; 
	
	msg.data[0] = index & 0xFF;	//Byte[0]��Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	
	/*
	//��������
	msg.data[4] = ;	
	msg.data[5] = ;
	msg.data[6] = ;
	msg.data[7] = ;
	*/
	//��������
	Send_Data_to_CyberGear(&msg);
	
	//���մ���������
	
}
#else 
/**
  * @brief  ���ģʽ����д�����ͨ������ 18������ģʽ�л���
  * @param  motor_id	���CAN ID(�ӻ�)
  * @param 	master_id	����CAN ID
  * @param 	index	������
  * @param 	runmode	����ģʽ  0: �˿�ģʽ 1: λ��ģʽ 2: �ٶ�ģʽ 3: ����ģʽ
  * @retval None
  */
void CyberGear_Change_Run_Mode(uint8_t motor_id, uint8_t master_id, uint16_t index, uint8_t runmode)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = WRITE_ONE_PARAM; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF; 
	msg.id.res = 0; 
	
	msg.data[0] = index & 0xFF;	//Byte[0]��Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	msg.data[4] = runmode;
	
	//��������
	Send_Data_to_CyberGear(&msg);
	
	//���մ���������
	
}

/**
  * @brief  ���ģʽ����д�����ͨ������ 18�����Ʋ���д�룩
  * @param  motor_id	���CAN ID(�ӻ�)
  * @param 	master_id	����CAN ID
  * @param 	index	������
  * @param 	ref		���Ʋ���
  * @retval None
  */
void CyberGear_Change_Control_Param(uint8_t motor_id, uint8_t master_id, uint16_t index, float ref)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = WRITE_ONE_PARAM; 
	msg.id.dst_id = motor_id; 
	msg.id.data = master_id & 0xFF; 
	msg.id.res = 0; 
	
	msg.data[0] = index & 0xFF;	//Byte[0]��Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	
	memcpy(&msg, &ref, 4);
	
	//��������
	Send_Data_to_CyberGear(&msg);
	
	//���մ���������
	
}
#endif


/**
  * @brief  �������CyberGear������ص�����
  * @param  None
  * @retval None
  */
void Decode_CyberGear_Ret_Data(CyberGear_Info_t *recv_data, uint8_t *motor_state, CyberGear_FeedBack_t *motor_info)
{
	DPRINTF("start decode!\n");
	extern uint8_t motor_id[4];
	extern uint8_t mcu_id[8];
	uint8_t state_byte;
	//���ݷ������ͽ������ݽ���
	switch(recv_data->id.mode)
	{
		case GET_MOTOR_ID:
			if(recv_data->id.dst_id == 0xFE)
			{
				motor_id[0] = recv_data->id.data;//Ŀ����ID
				for(int i=0; i<8; i++)
				{
					mcu_id[i] = recv_data->data[i];
				}
			}
			break;
		case MOTOR_FEEDBACK:
			//1���������ж��Ƿ��й���	
			state_byte = recv_data->id.data >> 8;		
			if(state_byte&0xC0)		//1100 0000
			{
				//Motor ģʽ[����], ��������
				*motor_state = OK;
			}else
			{
				for(int i = 1; i < 7; i++)
				{
					if(state_byte & 0x01)
					{
						*motor_state = i;
					}
					state_byte = state_byte >> 1;
				}
			}
			//2������ �Ƕȡ����ٶȡ�Ť�ء��¶�
			if((recv_data->id.data&0xFF) == motor_id[0])
			{
				//CyberGear_FeedBack_t motor_info;
				uint16_t decode_temp = 0;
				//�Ƕ�
				decode_temp = (recv_data->data[0] << 8 | recv_data->data[1])^0x8000;
				motor_info->angle = (float)decode_temp/32768*4*3.1415926f;
				//���ٶ�
				decode_temp = (recv_data->data[2] << 8 | recv_data->data[3])^0x8000;
				motor_info->speed = (float)decode_temp/32768*30;
				//����
				decode_temp = (recv_data->data[4] << 8 | recv_data->data[5])^0x8000;
				motor_info->torque = (float)decode_temp/32768*12.0f;				
				//�¶�
				decode_temp = (recv_data->data[6] << 8 | recv_data->data[7])^0x8000;
				motor_info->temperature = (float)decode_temp/10.0f;
			}
			break;
		default:
			
			break;
	}

}

/**
  * @brief  CAN1�����ж�
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg CAN1_RxMessage;
	
	//�յ�����
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_RxMessage);
		//�жϴ������
		#if 0
		DPRINTF("Recv:");
		for(int i = 0; i<8; i++)
		{
			DPRINTF("%02X", CAN1_RxMessage.Data[i]);
		}
		DPRINTF("\n");
		#else
		//���������������
		for(int i=0; i<8; i++)
		{
			recv_data.data[i] = CAN1_RxMessage.Data[i];
		}
		//�洢���յ�����չID
		recv_data.id = *(CyberGear_CAN_Id_t *)&CAN1_RxMessage.ExtId;
		//Decode_CyberGear_Ret_Data(&recv_data, &motor_state, &motor_info);	//������������Ϣ
		
		#endif
		//����жϱ�־λ
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}


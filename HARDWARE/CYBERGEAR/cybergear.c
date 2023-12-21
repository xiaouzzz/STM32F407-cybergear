#include "cybergear.h"

/*
小米CyberGear微电机驱动

电机通信为 CAN 2.0 通信接口，波特率 1Mbps，采用 扩展帧格式

电机支持的控制模式包括：
运控模式：给定电机运控 5 个参数；
电流模式：给定电机指定的 Iq 电流；
速度模式：给定电机指定的运行速度；
位置模式：给定电机指定的位置，电机将运行到该指定的位置

数据域				29 位 ID						8Byte数据区
大小 	Bit28~bit24   	 bit23~8    bit7~0 			Byte0~Byte7
描述 	通信类型 	 	 数据区 2   目标地址 		数据区 1

小米微电机通信数据均是 29位ID + 8字节 数据的格式

*/

CanTxMsg CAN1_TxMessage;
CanRxMsg CAN1_RxMessage;
uint8_t master_id = 100;
uint8_t motor_id[4] = {1,2,3,4};
uint8_t mcu_id[8] = {0};
uint8_t motor_state;	
CyberGear_Info_t recv_data;	//接收电机返回数据结构体
CyberGear_FeedBack_t motor_info;

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
	#if 0
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
  * @brief  float转int，数据打包用
  * @param  f_val		float数值
  * @param  f_val_min	float数值的最小值
  * @param  f_val_max	float数值的最大值
  * @param  int_bits	int的数据位数
  * @retval float数据转为int的值
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
  * @brief  小米微电机初始化
  * @param  None
  * @retval None
  */
void CyberGear_Init(void)
{
	uint16_t can_id;
	uint8_t mcu_id[8];
	DPRINTF("CyberGear Initing...\r\n");
	//CAN通信初始化
	CAN1_Init();
	//获取电机信息
	CyberGear_Id_get(&can_id, mcu_id);	
	CyberGear_Eanble_Run(motor_id[0], master_id);
	CyberGear_Eanble_Run(motor_id[1], master_id);
	CyberGear_Eanble_Run(motor_id[2], master_id);
	CyberGear_Eanble_Run(motor_id[3], master_id);
	//发送初始配置信息
}

/**
  * @brief  发送数据到小米微电机
  * @param  data 需要发送的数据
  * @retval 成功返回0，失败返回-1
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
  * @brief  获取设备 ID （通信类型 0） ；获取设备的 ID 和 64 位 MCU 唯一标识符
  * @param  can_id	电机CAN ID
  * @param 	mcu_id	电机MCU ID
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
	//发送数据
	int ret = Send_Data_to_CyberGear(&msg);
	DPRINTF("ret=%d\r\n",ret);
	//接收应答
	/*
		应答帧：	 ——————————————————————————————————————————————————————————————————————
		数据域 		|			   29 位 ID 					    |	8Byte 数据区   	   |
		大小 	 	| Bit28~bit24 |  bit23~8 bit7~0   |	Byte0~Byte7	|				   	   |
		描述 		|     0 	  |	 目标电机 CAN_ID  | 0XFE   		|  64 位MCU 唯一标识符 |
					|______________________________________________________________________|
	*/
	DPRINTF("FUNCTION:%s\tLINE:%d\r\n", __FUNCTION__, __LINE__);

}

/**
  * @brief  修改小米微电机 CAN通信ID (模式7)
  * @param  old_id	电机当前CAN ID
  * @param 	new_id	电机新的CAN ID
  * @param 	master_id	标识主机ID
  * @retval None
  */
void Change_CyberGear_CAN_Id(uint8_t old_id, uint8_t new_id, uint8_t master_id)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = SET_MOTOR_ID;	//模式7
	msg.id.dst_id = old_id;
	msg.id.data = new_id << 8 | master_id;	//id.data是bit8 ~ bit23
	msg.id.res = 0;
	
	for(int i=0; i<8; i++)
	{
		msg.data[i] = 0;
	}
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief	电机控制模式
  * @param  motor_id 电机当前CAN ID
  * @param  torque 	 电机扭距
  * @param 	MechPosition 目标角度
  * @param 	speed	 角速度
  * @param 	kp		 响应速度(到达位置快慢)，一般取1-10
  * @param 	kd		 电机阻尼，过小会震荡，过大电机会震动明显。一般取0.5左右
  * @retval None
  */
void CyberGear_Control_Mode(uint8_t motor_id, float torque, float MechPosition, float speed, float kp, float kd)
{
	CyberGear_Info_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.id.mode = MOTOR_MOVE_CNL;
	msg.id.dst_id = motor_id;	//目标电机ID
	msg.id.data = float_to_uint(torque, T_MIN, T_MAX, 16);		   //力矩 (0~65535)
	msg.id.res = 0;
	
	msg.data[0]=float_to_uint(MechPosition, P_MIN, P_MAX, 16)>>8;  //Byte0~1: 目标角度[0~65535],  对应(-4π~4π)
	msg.data[1]=float_to_uint(MechPosition, P_MIN, P_MAX, 16); 	
	msg.data[2]=float_to_uint(speed, V_MIN, V_MAX, 16)>>8;		   //Byte2~3: 目标角速度[0~65535],对应(-30rad/s~30rad/s)
	msg.data[3]=float_to_uint(speed, V_MIN, V_MAX, 16); 
	msg.data[4]=float_to_uint(kp, KP_MIN, KP_MAX, 16)>>8; 		   //Byte4~5：Kp [0~65535]对应 (0.0~500.0)
	msg.data[5]=float_to_uint(kp, KP_MIN, KP_MAX, 16); 
	msg.data[6]=float_to_uint(kd, KD_MIN, KD_MAX, 16)>>8; 		   //Byte6~7：Kd [0~65535]对应 (0.0~5.0)
	msg.data[7]=float_to_uint(kd, KD_MIN, KD_MAX, 16);
	
	Send_Data_to_CyberGear(&msg);
}

/**
  * @brief	修改电机扭矩
  * @param  motor_id 电机当前CAN ID
  * @param  torque 	 电机扭距
  * @retval None
  */
void CyberGear_Change_Torque(uint8_t motor_id, float torque)
{
	CyberGear_Control_Mode(motor_id, torque, 0.0, 0.0, 0.0, 0.0);
}

/**
  * @brief	电机运转到某一位置
  * @param  motor_id 电机当前CAN ID
  * @param 	MechPosition 目标角度
  * @param 	kp		 响应速度
  * @param 	kd		 电机阻尼
  * @retval None
  */
void CyberGear_Turn_Certain_Position(uint8_t motor_id, float MechPosition, float kp, float kd)
{
	CyberGear_Control_Mode(motor_id, 0.0, MechPosition, 0.0, kp, kd);
	//CyberGear_Control_Mode(motor_id, 0.0,  3.14, 0.0, 0.5, 0.5);	//正转1圈
	//CyberGear_Control_Mode(motor_id, 0.0, -3.14, 0.0, 0.5, 0.5);	//反转1圈
}

/**
  * @brief	电机控制模式
  * @param  motor_id 电机当前CAN ID
  * @param 	speed	 角速度(speed>0正转,speed<0反转)
  * @param 	kd		 电机阻尼
  * @retval None
  */
void CyberGear_Change_Speed(uint8_t motor_id, float speed, float kd)
{
	CyberGear_Control_Mode(motor_id, 0.0, 0.0, speed, 0.0, kd);
}

/**
  * @brief  电机使能运行
  * @param  motor_id	电机CAN ID
  * @param 	master_id	标识主机 CAN ID
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
  * @brief  电机停止运行
  * @param  motor_id	电机CAN ID
  * @param 	master_id	标识主机CAN ID
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
  * @brief  设置电机机械零位
  * @param  motor_id	电机CAN ID
  * @param 	master_id	标识主机CAN ID
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
  * @brief  读取单个参数
  * @param  motor_id	电机CAN ID(从机)
  * @param 	master_id	主机CAN ID
  * @param 	index	功能码
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
	
	msg.data[0] = index & 0xFF;	//Byte[0]、Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	for(int i=2; i<8; i++)
	{
		msg.data[i] = 00;
	}
	//发送请求
	Send_Data_to_CyberGear(&msg);
	
	//接收处理反回数据
	
}

#if 0
/**
  * @brief  单个参数写入
  * @param  motor_id	电机CAN ID(从机)
  * @param 	master_id	主机CAN ID
  * @param 	index	功能码
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
	
	msg.data[0] = index & 0xFF;	//Byte[0]、Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	
	/*
	//参数数据
	msg.data[4] = ;	
	msg.data[5] = ;
	msg.data[6] = ;
	msg.data[7] = ;
	*/
	//发送请求
	Send_Data_to_CyberGear(&msg);
	
	//接收处理反回数据
	
}
#else 
/**
  * @brief  电机模式参数写入命令（通信类型 18，运行模式切换）
  * @param  motor_id	电机CAN ID(从机)
  * @param 	master_id	主机CAN ID
  * @param 	index	功能码
  * @param 	runmode	运行模式  0: 运控模式 1: 位置模式 2: 速度模式 3: 电流模式
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
	
	msg.data[0] = index & 0xFF;	//Byte[0]、Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	msg.data[4] = runmode;
	
	//发送请求
	Send_Data_to_CyberGear(&msg);
	
	//接收处理反回数据
	
}

/**
  * @brief  电机模式参数写入命令（通信类型 18，控制参数写入）
  * @param  motor_id	电机CAN ID(从机)
  * @param 	master_id	主机CAN ID
  * @param 	index	功能码
  * @param 	ref		控制参数
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
	
	msg.data[0] = index & 0xFF;	//Byte[0]、Byte[1] :index
	msg.data[1] = (index>>8) & 0xFF;	
	msg.data[2] = 0x0;	//Byte2~3: 00
	msg.data[3] = 0x0;
	
	memcpy(&msg, &ref, 4);
	
	//发送请求
	Send_Data_to_CyberGear(&msg);
	
	//接收处理反回数据
	
}
#endif


/**
  * @brief  解码接收CyberGear电机返回的数据
  * @param  None
  * @retval None
  */
void Decode_CyberGear_Ret_Data(CyberGear_Info_t *recv_data, uint8_t *motor_state, CyberGear_FeedBack_t *motor_info)
{
	DPRINTF("start decode!\n");
	extern uint8_t motor_id[4];
	extern uint8_t mcu_id[8];
	uint8_t state_byte;
	//根据返回类型进行数据解析
	switch(recv_data->id.mode)
	{
		case GET_MOTOR_ID:
			if(recv_data->id.dst_id == 0xFE)
			{
				motor_id[0] = recv_data->id.data;//目标电机ID
				for(int i=0; i<8; i++)
				{
					mcu_id[i] = recv_data->data[i];
				}
			}
			break;
		case MOTOR_FEEDBACK:
			//1、解析并判断是否有故障	
			state_byte = recv_data->id.data >> 8;		
			if(state_byte&0xC0)		//1100 0000
			{
				//Motor 模式[运行], 运行正常
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
			//2、解析 角度、角速度、扭矩、温度
			if((recv_data->id.data&0xFF) == motor_id[0])
			{
				//CyberGear_FeedBack_t motor_info;
				uint16_t decode_temp = 0;
				//角度
				decode_temp = (recv_data->data[0] << 8 | recv_data->data[1])^0x8000;
				motor_info->angle = (float)decode_temp/32768*4*3.1415926f;
				//角速度
				decode_temp = (recv_data->data[2] << 8 | recv_data->data[3])^0x8000;
				motor_info->speed = (float)decode_temp/32768*30;
				//力矩
				decode_temp = (recv_data->data[4] << 8 | recv_data->data[5])^0x8000;
				motor_info->torque = (float)decode_temp/32768*12.0f;				
				//温度
				decode_temp = (recv_data->data[6] << 8 | recv_data->data[7])^0x8000;
				motor_info->temperature = (float)decode_temp/10.0f;
			}
			break;
		default:
			
			break;
	}

}

/**
  * @brief  CAN1接收中断
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg CAN1_RxMessage;
	
	//收到数据
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_RxMessage);
		//中断处理操作
		#if 0
		DPRINTF("Recv:");
		for(int i = 0; i<8; i++)
		{
			DPRINTF("%02X", CAN1_RxMessage.Data[i]);
		}
		DPRINTF("\n");
		#else
		//解析电机返回数据
		for(int i=0; i<8; i++)
		{
			recv_data.data[i] = CAN1_RxMessage.Data[i];
		}
		//存储接收到的扩展ID
		recv_data.id = *(CyberGear_CAN_Id_t *)&CAN1_RxMessage.ExtId;
		//Decode_CyberGear_Ret_Data(&recv_data, &motor_state, &motor_info);	//解码电机返回信息
		
		#endif
		//清除中断标志位
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}


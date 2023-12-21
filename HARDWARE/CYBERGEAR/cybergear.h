#ifndef __CYBERGEAR_H
#define __CYBERGEAR_H

#ifdef __cplusplus
extern {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "debug.h"


#define P_MIN 	-12.5f 
#define P_MAX 	12.5f 
#define V_MIN 	-30.0f 
#define V_MAX 	30.0f 
#define KP_MIN 	0.0f
#define KP_MAX 	500.0f 
#define KD_MIN 	0.0f 
#define KD_MAX 	5.0f 
#define T_MIN 	-12.0f 
#define T_MAX 	12.0f

//4������ģʽ
#define RUN_CNTL_MODE 0	//�˿�ģʽ
#define POSITION_MODE 1	//λ��ģʽ
#define SPEED_MODE	  2 //�ٶ�ģʽ
#define CURRENT_MODE  3 //����ģʽ


//�ɶ�д���������б�궨��
#define RUN_MODE 	  0x7005 //0: �˿�ģʽ 1: λ��ģʽ 2: �ٶ�ģʽ 3: ����ģʽ
#define IQ_REF	 	  0x7006 //����ģʽIqָ��
#define SPD_REF	 	  0x700A //ת��ģʽת��ָ��
#define LIMIT_TORQUE  0x700B //ת������
#define CUR_KP		  0x7010 //������ Kp
#define CUR_KI		  0x7011 //������ Ki
#define CUR_FILT_GAIN 0x7014 //�����˲�ϵ��filt_gain
#define LOC_REF		  0x7016 //λ��ģʽ�Ƕ�ָ��
#define LIMIT_SPD	  0x7017 //λ��ģʽ�ٶ�����
#define LIMIT_CUR	  0x7018 //�ٶ�λ��ģʽ��������



//С��΢���ʹ��CAN��32λ��չID
typedef struct{
	uint32_t  dst_id:8;	// Bit 0  - Bit 7, 	8λ��ʾĿ���ַ 
    uint32_t  data:16;	// Bit 8  - Bit 23,	16λ��ʾ������  
	uint32_t  mode:5;	// Bit 24 - Bit 28,	5λ��ʾͨ������  
	uint32_t  res:3;	// Bit 29 - Bit 32,	3λ
}CyberGear_CAN_Id_t;

//CyberGearͨ�����ݸ�ʽ
typedef struct{
	CyberGear_CAN_Id_t id; //ID
	uint8_t data[8];	   //8 Byte������	
}CyberGear_Info_t;

typedef struct{
	float angle;
	float speed;
	float torque;
	float temperature;
}CyberGear_FeedBack_t; 

//CyberGear�������ģʽ
enum MOTOR_MODE_TYPE{
	GET_MOTOR_ID	= 0, //��ȡ�豸 ID
	MOTOR_MOVE_CNL	= 1, //�˿�ģʽ�������ָ��
	MOTOR_FEEDBACK	= 2, //�����������
	MOTOR_RUN		= 3, //���ʹ������
	MOTOR_STOP		= 4, //���ֹͣ����
	MOTOR_ZERO_POS	= 6, //���õ����е��λ
	SET_MOTOR_ID	= 7, //���õ�� CAN_ID
	READ_ONE_PARAM	= 17,//����������ȡ 
	WRITE_ONE_PARAM	= 18,//��������д��
	ERR_FEEDBACK	= 21 //���Ϸ���֡
};

enum MOTOR_RUN_STATE{
	OK				  = 0, //����
	LOW_VOLTAGE 	  = 1, //Ƿѹ����
	OVER_CURRENT 	  = 2, //����
	OVER_TEMP 		  = 3, //����
	MAGNETIC_CODE_ERR = 4, //�ű������
	HALL_CODE_ERR 	  = 5, //HALL�������
	UNMARKED		  = 6  //δ�궨
};

enum MOTOR_MODE{
	RESET_Mode = 0,	//Reset ģʽ[��λ]
	CALI_Mode  = 1,	//Cali  ģʽ[�궨]
	RUN_Mode   = 2	//Motor ģʽ[����]
};



extern CanTxMsg CAN1_TxMessage;
extern CanRxMsg CAN1_RxMessage;
extern uint8_t master_id;
extern uint8_t motor_id[4];
extern uint8_t mcu_id[8];
extern uint8_t motor_state;
extern CyberGear_Info_t recv_data;	//���յ���������ݽṹ
extern CyberGear_FeedBack_t motor_info;

void CAN1_Init(void);
int  float_to_uint(float f_val, float f_val_min, float f_val_max, int int_bits);
void CyberGear_Init(void);
int  Send_Data_to_CyberGear(CyberGear_Info_t *data);
void CyberGear_Id_get(uint16_t *can_id, uint8_t *mcu_id);
void Change_CyberGear_CAN_Id(uint8_t old_id, uint8_t new_id, uint8_t master_id);
void CyberGear_Control_Mode(uint8_t motor_id, float torque, float MechPosition, float speed, float kp, float kd); 
void CyberGear_Change_Torque(uint8_t motor_id, float torque);
void CyberGear_Turn_Certain_Position(uint8_t motor_id, float MechPosition, float kp, float kd);
void CyberGear_Change_Speed(uint8_t motor_id, float speed, float kd);
void CyberGear_Eanble_Run(uint8_t motor_id, uint8_t master_id);
void CyberGear_Stop_Run(uint8_t motor_id, uint8_t master_id);
void Set_CyberGear_Zero_Position(uint8_t motor_id, uint8_t master_id);
void Read_CyberGear_Single_Param(uint8_t motor_id, uint8_t master_id, uint16_t index);
void Write_CyberGear_Single_Param(uint8_t motor_id, uint8_t master_id, uint16_t index);
void Decode_CyberGear_Ret_Data(CyberGear_Info_t *recv_data, uint8_t *motor_state, CyberGear_FeedBack_t *motor_info);
void CAN1_RX0_IRQHandler(void);



#ifdef __cplusplus
}
#endif
#endif


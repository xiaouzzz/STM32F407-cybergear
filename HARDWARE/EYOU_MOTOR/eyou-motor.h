#ifndef	__EYOU_MOTOR_H
#define __EYOU_MOTOR_H

#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "debug.h"

//������ݽṹ��
typedef struct{
	uint8_t motor_id;	//���ID
	int dataLen;		//�跢�����ݳ���
	//uint8_t data[8];
	uint8_t cmd;		//��������
	uint8_t addr;		//ָ���ַ���Ĵ�����ַ
	uint8_t data[6];	//6 Byte������
}Eyou_Motor_Data_t;

//���������������
enum Cmd_Mode{
	NET_MANAGE = 0x00,	//�������
	WRITE_CMD  = 0x01,	//д������
	WRITE_RET  = 0x02,	//д�������
	READ_CMD   = 0x03,	//��ȡ����
	READ_RET   = 0x04,	//��ȡ�����
	FAST_WRITE = 0x05	//��д����
};

//����д����
enum Write_Ret_State{
	WRITE_FAILED  	  = 0x00,	//д��ʧ��
	WRITE_OK          = 0x01,	//д��ɹ�
	WRITE_DATA_ERR    = 0x03,	//д������ݳ���
	FAIL_DATA_EXCEED  = 0x04,	//���ݳ�����Χ��д��ʧ��
	OK_OUT_DATA_AMEND = 0x05	//���ݳ�����Χ��������д��ɹ�
};

//�������ģʽ
enum Motor_Work_Mode{
	OUTLINE_POS_MODE_SPEED = 0x01,	//����λ��ģʽ-�ٶ�
	OUTLINE_POS_MODE_TIME  = 0x02,	//����λ��ģʽ-ʱ��
	OUTLINE_SPEED_MODE	   = 0x03,	//�����ٶ�ģʽ
	CURRENT_MODE 		   = 0x04,	//����ģʽ
	PERIOD_SYN_POS_MODE	   = 0x05	//����ͬ��λ��ģʽ
};

//�澯ָʾ
enum Warn_Instruction{
	OVER_VOLTAGE	= 0x0001,	//��ѹ����
	LOW_VOLTAGE		= 0x0002,	//Ƿѹ����
	OVER_TEMP		= 0x0004,	//���±���
	OVER_CURRENT	= 0x0010,	//��������
	OVER_LOAD		= 0x0020,	//���ر���
	MOTOR_LOCK		= 0x0040,	//���������
	LACK_PHASE		= 0x0080,	//ȱ�ౣ��
	MAGNETIC_ERROR	= 0x0200,	//�ű������ų�����
	MAGNETIC_LOW_VOL= 0x0400,	//�ű�����Ƿѹ
	ANGLE_ERROR		= 0x0800,	//�ű�Ƕȳ���
	PARAM_R_W_ERR	= 0x0100	//������д�쳣
};

//Э��ָ���ַ�б�
#define DEVICE_SERIAL_NUM	0x02	//�豸���к�
#define DEVICE_HARDWARE_VER	0x03	//�豸Ӳ���汾��
#define DEVICE_FIRMWARE_VER	0x04	//�豸�̼��汾��
#define CURRENT_VALUE		0x05	//��ǰ����ֵ
#define SPEED_VALUE			0x06	//��ǰ�ٶ�ֵ
#define POSITION_VALUE		0x07	//��ǰλ��ֵ
#define SET_CURRENT_VALUE 	0x08	//����Ŀ�����ֵ
#define	SET_SPEED_VALUE		0x09	//����Ŀ���ٶ�ֵ
#define SET_POSITION_VALUE	0x0A	//����Ŀ��λ��ֵ
#define SET_ACCELER_VALUE   0x0B	//����Ŀ����ٶ�ֵ
#define	SET_SUB_SPEED		0x0C	//����Ŀ����ٶ�ֵ
#define	MOTOR_WORK_MODE		0x0F	//��ǰ����ģʽ
#define EN_DIS_WORK_STATE	0x10	//ʹ��/ʧ��״̬
#define CLOSE_RUN_STATE		0x11	//������ǰ����״̬
#define WARN_INSTRUCTION	0x15	//�澯ָʾ
#define ELECTRIC_GEAR_RATIO 0x1A	//���ӳ��ֱ�
#define BUS_VOLTAGE			0x1B	//��ǰĸ�ߵ�ѹֵ
#define BUS_PROTECT_VOLTAGE 0x1C	//ĸ�߱���������ѹ
#define MOTOR_TEMPERATURE	0x1D	//��ǰ�¶�ֵ
#define MOTOR_PROTECT_TEMP	0x1E	//���������¶�
#define MOTOR_RES_CTL_TEMP	0x1F	//�����ָ��¶�
#define CURRENT_LOOP_P_VAL	0x23	//�������� P ֵ
#define CURRENT_LOOP_I_VAL  0x24	//�������� I ֵ
#define SPEED_LOOP_P_VAL	0x25	//�ٶȻ��� P ֵ
#define SPEED_LOOP_I_VAL	0x26	//�ٶȻ��� I ֵ
#define POSITION_LOOP_P_VAL	0x27	//λ�û��� P ֵ
#define POSITION_LOOP_I_VAL	0x28	//λ�û��� I ֵ
#define TORQUE_LOOP_RESTRIC	0x29	//���ػ���������
#define MAX_CURRENT_LIMIT	0x30	//�������������
#define MAX_SPEED_LIMIT		0x31	//������ٶ�����
#define SPEED_CURVE_MAX_VAL	0x32	//�ٶ��������ߵ�����ٶ�ֵ
#define SPEED_CURVE_ACE_MAX	0x33	//�ٶ��������ߵļ��ٶ����ֵ
#define SPEED_CURVE_SUB_MAX	0x34	//�ٶ��������ߵļ��ٶ����ֵ
#define POS_CURVE_MAX_VAL	0x35	//λ���������ߵ�����ٶ�ֵ
#define POS_CURVE_ACE_MAX	0x36	//λ���������ߵļ��ٶ����ֵ
#define POS_CURVE_SUB_MAX	0x37	//λ���������ߵļ��ٶ����ֵ
#define POS_LIMIT_STATE		0x38	//λ����λ״̬
#define POS_MAX_LIMIT_VAL	0x39	//λ�õ�����ֵ
#define POS_MIN_LIMIT_VAL	0x3A	//λ�õ�����ֵ
#define POS_OFFSET_PARA_VAL 0x3B	//λ�õ�ƫ�ò���ֵ
#define SINGLE_LP_POS_RANGE	0x3C	//�ϵ�ʱ�̵�Ȧλ�÷�Χֵ
#define CHANGE_CAN_BAUDRATE	0x4B	//�޸� CAN ������
#define CHANGE_DEV_CAN_ID	0x4C	//�޸��豸�� CANID
#define SAVE_DATA			0x4D	//���ݱ���

void CAN1_Init(void);
int8_t Send_Data_to_eyou_Motor(Eyou_Motor_Data_t *msg);
uint8_t get_eyou_motor_id(void);


#ifdef __cplusplus
}
#endif

#endif

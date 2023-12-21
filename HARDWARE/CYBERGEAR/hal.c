
/**
 *
 * @File:        MI_motor_dev.c
 * @Author:      ���˲�˧
 *
 */
/* Includes -------------------------------------------------------------------*/
#include "hal.h"

uint8_t MI_MASTERID = 1; //master id ����ָ��ʱEXTID��bit8:15,������bit0:7
uint8_t MI_fdbid = 0;//����ID����ȡ���ID��ʶ������
uint8_t MI_MCU_identifier[8];
MI_Motor_t MI_Motor;
/**
  * @brief  floatתint�����ݴ����
  * @param  x float��ֵ
  * @param  x_min float��ֵ����Сֵ
  * @param  x_max float��ֵ�����ֵ
  * @param  bits  int������λ��
  * @retval null
  */
int float_to_uint(float x, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    if(x > x_max) x=x_max;
    else if(x < x_min) x= x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}
/**
  * @brief  С�׵��CANͨ�ŷ���
  * @param  hmotor ����ṹ��
  * @retval null
  */
CAN_TxHeaderTypeDef CAN_TxHeader_MI;
void MI_Motor_CanTx(MI_Motor_t* hmotor) {
 
    CAN_TxHeader_MI.DLC = 8;
    CAN_TxHeader_MI.IDE = CAN_ID_EXT;
    CAN_TxHeader_MI.RTR = CAN_RTR_DATA;
    CAN_TxHeader_MI.ExtId = *((uint32_t*)&(hmotor->EXT_ID));
	/*CAN_TxHeader_MI.ExtId = hmotor->EXT_ID.motor_id<<24 | hmotor->EXT_ID.data << 8 |          hmotor->EXT_ID.mode << 5;*/
    uint32_t mailbox;
    /* Start the Transmission process */
    uint32_t ret = HAL_CAN_AddTxMessage(hmotor->phcan, &CAN_TxHeader_MI, hmotor->txdata, &mailbox);
    if (ret != HAL_OK) {
        /* Transmission request Error */
        while(1);
    }
}
/**
  * @brief  С�׵����ʼ��
  * @param  hmotor ����ṹ��
  * @param  phcan can���߾��
  * @retval null
  */
void MI_motor_init(MI_Motor_t* hmotor,CAN_HandleTypeDef *phcan)
{
    hmotor->phcan = phcan;
}
/**
  * @brief  С�׵��ʹ��
  * @param  hmotor ����ṹ��
  * @param  id ���id
  * @retval null
  */
void MI_motor_enable(MI_Motor_t* hmotor,uint8_t id)
{
    hmotor->EXT_ID.mode = 3;
    hmotor->EXT_ID.motor_id = id;
    hmotor->EXT_ID.data = MI_MASTERID;
    hmotor->EXT_ID.res = 0;
    for(uint8_t i=0; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ��ȡ�豸ID ��ͨ������0�������ڵ��ʹ��ǰʹ��
  * @param  hmotor ����ṹ��
  * @retval null
  */
void MI_motor_get_ID(MI_Motor_t* hmotor)
{
    hmotor->EXT_ID.mode = 0;
    hmotor->EXT_ID.data = 0;
    hmotor->EXT_ID.motor_id = 0;
    hmotor->EXT_ID.res = 0;
 
    for(uint8_t i=0; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  �˿�ģʽ�������ָ�ͨ������1��
  * @param  hmotor ����ṹ��
  * @param  motor_id ���id
  * @param  master_id ����id
  * @retval null
  */
void MI_motor_controlmode(MI_Motor_t* hmotor, float torque, float MechPosition , float speed , float kp , float kd)
{
    hmotor->EXT_ID.mode = 1;
    hmotor->EXT_ID.data = float_to_uint(torque,T_MIN,T_MAX,16);
    hmotor->EXT_ID.res = 0;
 
    hmotor->txdata[0]=float_to_uint(MechPosition,P_MIN,P_MAX,16)>>8;
    hmotor->txdata[1]=float_to_uint(MechPosition,P_MIN,P_MAX,16);
    hmotor->txdata[2]=float_to_uint(speed,V_MIN,V_MAX,16)>>8;
    hmotor->txdata[3]=float_to_uint(speed,V_MIN,V_MAX,16);
    hmotor->txdata[4]=float_to_uint(kp,KP_MIN,KP_MAX,16)>>8;
    hmotor->txdata[5]=float_to_uint(kp,KP_MIN,KP_MAX,16);
    hmotor->txdata[6]=float_to_uint(kd,KD_MIN,KD_MAX,16)>>8;
    hmotor->txdata[7]=float_to_uint(kd,KD_MIN,KD_MAX,16);
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ���ֹͣ����֡��ͨ������4��
  * @param  hmotor ����ṹ��
  * @retval null
  */
void MI_motor_stop(MI_Motor_t* hmotor)
{
    hmotor->EXT_ID.mode = 4;
    hmotor->EXT_ID.data = MI_MASTERID;
    hmotor->EXT_ID.res = 0;
 
    for(uint8_t i=0; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ���õ����е��λ��ͨ������6����ѵ�ǰ���λ����Ϊ��е��λ�����綪ʧ��
  * @param  hmotor ����ṹ��
  * @retval null
  */
void MI_motor_setMechPosition2Zero(MI_Motor_t* hmotor)
{
    hmotor->EXT_ID.mode = 6;
    hmotor->EXT_ID.data = MI_MASTERID;
    hmotor->EXT_ID.res = 0;
    hmotor->txdata[0]=1;
 
    for(uint8_t i=1; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ���õ��CAN_ID��ͨ������7�����ĵ�ǰ���CAN_ID , ������Ч�����ڵ��ʹ��ǰʹ��
  * @param  hmotor ����ṹ��
  * @param  Now_ID ������ڵ�ID
  * @param  Target_ID ��Ҫ�ĳɵĵ��ID
  * @retval null
  */
void MI_motor_changeID(MI_Motor_t* hmotor,uint8_t Now_ID,uint8_t Target_ID)
{
    hmotor->EXT_ID.mode = 7;	
    hmotor->EXT_ID.motor_id = Now_ID;
    hmotor->EXT_ID.data = Target_ID << 8 | MI_MASTERID;
    hmotor->EXT_ID.res = 0;
 
    for(uint8_t i=0; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ����������ȡ��ͨ������17��
  * @param  hmotor ����ṹ��
  * @param  index ������
  * @retval null
  * @note   ���ò��ţ�����ûд��������
  */
void MI_motor_Read_One_Para(MI_Motor_t* hmotor,uint16_t index)
{
    hmotor->EXT_ID.mode = 17;
    hmotor->EXT_ID.data = MI_MASTERID;
    hmotor->EXT_ID.res = 0;
    hmotor->txdata[0]=index;
    memcpy(&hmotor->txdata[0],&index,2);
    for(uint8_t i=2; i<8; i++)
    {
        hmotor->txdata[i]=0;
    }
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ��������д�루ͨ������18�� �����綪ʧ��
  * @param  hmotor ����ṹ��
  * @param  index ������
  * @param  data[4] �������ݻ���
  * @retval null
  * @note   ���ò��ţ�����ûд��������
  */
void MI_motor_Write_One_Para(MI_Motor_t* hmotor, uint16_t index ,uint8_t data[4])
{
    hmotor->EXT_ID.mode = 0x12;
    hmotor->EXT_ID.data = MI_MASTERID;
    hmotor->EXT_ID.res = 0;
 
    memcpy(&hmotor->txdata[0],&index,2);
    memcpy(&hmotor->txdata[4],data, 4);
    MI_Motor_CanTx(hmotor);
}
/**
  * @brief  ���������
  * @param  hmotor ����ṹ��
  * @param  state_byte״̬�ֽڣ���չID��bit8:23
  * @param  rxdata ���ݻ�����
  * @retval null
  */
uint16_t decode_temp_mi = 0;
uint8_t nsvd = 0;
void MI_motor_decode(MI_Motor_t* hmotor,uint8_t state_byte,uint8_t rxdata[]) {
    nsvd = state_byte;
    if((state_byte&0xC0) == 0) {
        hmotor->motor_state = OK;
    } else {
        for(int i = 1; i < 7; i++) {
            if(state_byte&0x01) {
                hmotor->motor_state = i;
            }
            state_byte = state_byte>> 1;
        }
    }
    hmotor->motor_mode = state_byte;
 
    decode_temp_mi = (rxdata[0] << 8 | rxdata[1])^0x8000;
    hmotor->motor_fdb.angle_temp   = decode_temp_mi;
 
    decode_temp_mi = (rxdata[2] << 8 | rxdata[3])^0x8000;
    hmotor->motor_fdb.speed_temp   = decode_temp_mi;
 
    decode_temp_mi = (rxdata[4] << 8 | rxdata[5])^0x8000;
    hmotor->motor_fdb.torque_temp   = decode_temp_mi;
 
    decode_temp_mi = (rxdata[6] << 8 | rxdata[7]);
    hmotor->motor_fdb.temprature_temp  = decode_temp_mi;
 
    hmotor->motor_fdb.angle = (float)hmotor->motor_fdb.angle_temp/32768*4*3.1415926f;
    hmotor->motor_fdb.speed = (float)hmotor->motor_fdb.speed_temp/32768*30;
    hmotor->motor_fdb.torque = (float)hmotor->motor_fdb.torque_temp/32768*12.0f;
    hmotor->motor_fdb.temprature = (float)hmotor->motor_fdb.temprature_temp/10.0f;
 
    hmotor->motor_fdb.last_update_time = HAL_GetTick();
}
/**
  * @brief  С�׵������
  * @param  rx_EXT_id ���յ�����չID
  * @param  rxdata ���ݻ�����
  * @retval null
  */
EXT_ID_t EXT_ID_tmp;//��չID���ݽṹ��
void MIMotor_MotorDataDecode(uint32_t rx_EXT_id,uint8_t rxdata[])
{   EXT_ID_tmp = *((EXT_ID_t*)(&rx_EXT_id));
    if(EXT_ID_tmp.mode == 0&&EXT_ID_tmp.motor_id == 0xFE) {
        MI_fdbid = EXT_ID_tmp.data;
        memcpy(MI_MCU_identifier,rxdata, 8);
    }
    if(EXT_ID_tmp.mode == 2) {
        uint8_t id = EXT_ID_tmp.data&0xFF;
        if(id == MI_Motor.EXT_ID.motor_id) {
            MI_motor_decode(&MI_Motor,(uint8_t)(EXT_ID_tmp.data>>8),rxdata);
        }
    }
}


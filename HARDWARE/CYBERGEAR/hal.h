#ifndef MI_DEV_H
#define MI_DEV_H

#ifdef __cplusplus
extern "C"
{
#endif

//#include "pid_lib.h"//pid�㷨�⣬���Բ�Ҫ
#include "stm32f4xx.h"
#include "stdlib.h"

#define P_MIN -12.5f
#define P_MAX 12.5f
#define V_MIN -30.0f
#define V_MAX 30.0f
#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f
#define T_MIN -12.0f
#define T_MAX 12.0f

    typedef enum
    {
        OK                 = 0,//�޹���
        BAT_LOW_ERR        = 1,//Ƿѹ����
        OVER_CURRENT_ERR   = 2,//����
        OVER_TEMP_ERR      = 3,//����
        MAGNETIC_ERR       = 4,//�ű������
        HALL_ERR_ERR       = 5,//HALL�������
        NO_CALIBRATION_ERR = 6//δ�궨
    } motor_state_e;//���״̬��������Ϣ��
	
    typedef enum
    {
        RESET_MODE = 0,//Reset[ģʽ]
        CALI_MODE  = 1,//Cali ģʽ[�궨]
        RUN_MODE   = 2//Motorģʽ[����]
    } motor_mode_e;//�������ģʽ
	
    typedef struct
    {
        uint32_t motor_id : 8; // ֻռ8λ
        uint32_t data : 16;
        uint32_t mode : 5;
        uint32_t res : 3;
    } EXT_ID_t; // 32λ��չID�����ṹ��
	
    typedef struct
    {
        // �������
        int16_t angle_temp;
        int16_t speed_temp;
        int16_t torque_temp;
        int16_t temprature_temp;
 
        float angle; // ������
        float speed;
        float torque;
        float temprature;
        uint32_t last_update_time; // ������ʱ���
    } Motor_fdb_t;                 // ��������������ṹ��
	
    typedef struct
    {
        CAN_HandleTypeDef *phcan;
        motor_state_e motor_state;
        motor_mode_e  motor_mode;
        EXT_ID_t EXT_ID;
        uint8_t txdata[8];
        //PID_t PID;
        Motor_fdb_t motor_fdb;
    } MI_Motor_t;
	
    /**********************Functions*************************8*/
    void MI_motor_get_ID(MI_Motor_t* hmotor);
    void MI_motor_init(MI_Motor_t* hmotor,CAN_HandleTypeDef *phcan);
    void MI_motor_enable(MI_Motor_t *hmotor, uint8_t id);
    void MI_motor_controlmode(MI_Motor_t* hmotor, float torque, float MechPosition , float speed , float kp , float kd);
    void MI_motor_stop(MI_Motor_t *hmotor);
    void MI_motor_setMechPosition2Zero(MI_Motor_t *hmotor);
    void MI_motor_changeID(MI_Motor_t* hmotor,uint8_t Now_ID,uint8_t Target_ID);
    void MIMotor_MotorDataDecode(uint32_t rx_EXT_id, uint8_t rxdata[]);
 
    extern MI_Motor_t MI_Motor;
	
#endif
 
// #endif
#ifdef __cplusplus
}
#endif
	

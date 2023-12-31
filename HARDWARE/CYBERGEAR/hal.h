#ifndef MI_DEV_H
#define MI_DEV_H

#ifdef __cplusplus
extern "C"
{
#endif

//#include "pid_lib.h"//pid算法库，可以不要
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
        OK                 = 0,//无故障
        BAT_LOW_ERR        = 1,//欠压故障
        OVER_CURRENT_ERR   = 2,//过流
        OVER_TEMP_ERR      = 3,//过温
        MAGNETIC_ERR       = 4,//磁编码故障
        HALL_ERR_ERR       = 5,//HALL编码故障
        NO_CALIBRATION_ERR = 6//未标定
    } motor_state_e;//电机状态（故障信息）
	
    typedef enum
    {
        RESET_MODE = 0,//Reset[模式]
        CALI_MODE  = 1,//Cali 模式[标定]
        RUN_MODE   = 2//Motor模式[运行]
    } motor_mode_e;//电机运行模式
	
    typedef struct
    {
        uint32_t motor_id : 8; // 只占8位
        uint32_t data : 16;
        uint32_t mode : 5;
        uint32_t res : 3;
    } EXT_ID_t; // 32位扩展ID解析结构体
	
    typedef struct
    {
        // 电机反馈
        int16_t angle_temp;
        int16_t speed_temp;
        int16_t torque_temp;
        int16_t temprature_temp;
 
        float angle; // 连续角
        float speed;
        float torque;
        float temprature;
        uint32_t last_update_time; // 编码器时间戳
    } Motor_fdb_t;                 // 电机编码器反馈结构体
	
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
	

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

//电机数据结构体
typedef struct{
	uint8_t motor_id;	//电机ID
	int dataLen;		//需发送数据长度
	//uint8_t data[8];
	uint8_t cmd;		//命令类型
	uint8_t addr;		//指令地址、寄存器地址
	uint8_t data[6];	//6 Byte数据区
}Eyou_Motor_Data_t;

//电机操作命令类型
enum Cmd_Mode{
	NET_MANAGE = 0x00,	//网络管理
	WRITE_CMD  = 0x01,	//写入命令
	WRITE_RET  = 0x02,	//写入命令返回
	READ_CMD   = 0x03,	//读取命令
	READ_RET   = 0x04,	//读取命令返回
	FAST_WRITE = 0x05	//快写命令
};

//命令写返回
enum Write_Ret_State{
	WRITE_FAILED  	  = 0x00,	//写入失败
	WRITE_OK          = 0x01,	//写入成功
	WRITE_DATA_ERR    = 0x03,	//写入的数据出错
	FAIL_DATA_EXCEED  = 0x04,	//数据超出范围，写入失败
	OK_OUT_DATA_AMEND = 0x05	//数据超出范围，矫正后写入成功
};

//电机工作模式
enum Motor_Work_Mode{
	OUTLINE_POS_MODE_SPEED = 0x01,	//轮廓位置模式-速度
	OUTLINE_POS_MODE_TIME  = 0x02,	//轮廓位置模式-时间
	OUTLINE_SPEED_MODE	   = 0x03,	//轮廓速度模式
	CURRENT_MODE 		   = 0x04,	//电流模式
	PERIOD_SYN_POS_MODE	   = 0x05	//周期同步位置模式
};

//告警指示
enum Warn_Instruction{
	OVER_VOLTAGE	= 0x0001,	//过压保护
	LOW_VOLTAGE		= 0x0002,	//欠压保护
	OVER_TEMP		= 0x0004,	//过温保护
	OVER_CURRENT	= 0x0010,	//过流保护
	OVER_LOAD		= 0x0020,	//过载保护
	MOTOR_LOCK		= 0x0040,	//电机锁保护
	LACK_PHASE		= 0x0080,	//缺相保护
	MAGNETIC_ERROR	= 0x0200,	//磁编码器磁场出错
	MAGNETIC_LOW_VOL= 0x0400,	//磁编码器欠压
	ANGLE_ERROR		= 0x0800,	//磁编角度出错
	PARAM_R_W_ERR	= 0x0100	//参数读写异常
};

//协议指令地址列表
#define DEVICE_SERIAL_NUM	0x02	//设备序列号
#define DEVICE_HARDWARE_VER	0x03	//设备硬件版本号
#define DEVICE_FIRMWARE_VER	0x04	//设备固件版本号
#define CURRENT_VALUE		0x05	//当前电流值
#define SPEED_VALUE			0x06	//当前速度值
#define POSITION_VALUE		0x07	//当前位置值
#define SET_CURRENT_VALUE 	0x08	//设置目标电流值
#define	SET_SPEED_VALUE		0x09	//设置目标速度值
#define SET_POSITION_VALUE	0x0A	//设置目标位置值
#define SET_ACCELER_VALUE   0x0B	//设置目标加速度值
#define	SET_SUB_SPEED		0x0C	//设置目标减速度值
#define	MOTOR_WORK_MODE		0x0F	//当前工作模式
#define EN_DIS_WORK_STATE	0x10	//使能/失能状态
#define CLOSE_RUN_STATE		0x11	//结束当前运行状态
#define WARN_INSTRUCTION	0x15	//告警指示
#define ELECTRIC_GEAR_RATIO 0x1A	//电子齿轮比
#define BUS_VOLTAGE			0x1B	//当前母线电压值
#define BUS_PROTECT_VOLTAGE 0x1C	//母线保护工作电压
#define MOTOR_TEMPERATURE	0x1D	//当前温度值
#define MOTOR_PROTECT_TEMP	0x1E	//工作保护温度
#define MOTOR_RES_CTL_TEMP	0x1F	//工作恢复温度
#define CURRENT_LOOP_P_VAL	0x23	//电流环的 P 值
#define CURRENT_LOOP_I_VAL  0x24	//电流环的 I 值
#define SPEED_LOOP_P_VAL	0x25	//速度环的 P 值
#define SPEED_LOOP_I_VAL	0x26	//速度环的 I 值
#define POSITION_LOOP_P_VAL	0x27	//位置环的 P 值
#define POSITION_LOOP_I_VAL	0x28	//位置环的 I 值
#define TORQUE_LOOP_RESTRIC	0x29	//力矩环积分限制
#define MAX_CURRENT_LIMIT	0x30	//最大工作电流限制
#define MAX_SPEED_LIMIT		0x31	//最大工作速度限制
#define SPEED_CURVE_MAX_VAL	0x32	//速度梯形曲线的最大速度值
#define SPEED_CURVE_ACE_MAX	0x33	//速度梯形曲线的加速度最大值
#define SPEED_CURVE_SUB_MAX	0x34	//速度梯形曲线的减速度最大值
#define POS_CURVE_MAX_VAL	0x35	//位置梯形曲线的最大速度值
#define POS_CURVE_ACE_MAX	0x36	//位置梯形曲线的加速度最大值
#define POS_CURVE_SUB_MAX	0x37	//位置梯形曲线的减速度最大值
#define POS_LIMIT_STATE		0x38	//位置限位状态
#define POS_MAX_LIMIT_VAL	0x39	//位置的上限值
#define POS_MIN_LIMIT_VAL	0x3A	//位置的下限值
#define POS_OFFSET_PARA_VAL 0x3B	//位置的偏置参数值
#define SINGLE_LP_POS_RANGE	0x3C	//上电时刻单圈位置范围值
#define CHANGE_CAN_BAUDRATE	0x4B	//修改 CAN 波特率
#define CHANGE_DEV_CAN_ID	0x4C	//修改设备的 CANID
#define SAVE_DATA			0x4D	//数据保存

void CAN1_Init(void);
int8_t Send_Data_to_eyou_Motor(Eyou_Motor_Data_t *msg);
uint8_t get_eyou_motor_id(void);


#ifdef __cplusplus
}
#endif

#endif

#include "stm32f4xx.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "key.h"
#include "cybergear.h"
#include "MPU6050.h"
#if 1
/**
  * @brief  小米电机测试主函数
  * @param  None
  * @retval None
  */
 


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 设置系统中断优先级分组2
	USART1_Init(115200);
	DPRINTF("CyberGear run test...\r\n");
	DPRINTF("FUNCTION:%s\tLINE:%d\r\n", __FUNCTION__, __LINE__);
	
	
//	CyberGear_Init();
	
//	CyberGear_Turn_Certain_Position(motor_id[0], 0.3, 0.5, 0.5);	//正转1圈
//	CyberGear_Turn_Certain_Position(motor_id[1], 0.3, 0.5, 0.5);	//反转1圈
//	CyberGear_Turn_Certain_Position(motor_id[2], 0.3, 0.5, 0.5);	//正转1圈
//	CyberGear_Turn_Certain_Position(motor_id[3], 0.3, 0.5, 0.5);	//反转1圈
//	
//	delay_ms(200);
//	CyberGear_Turn_Certain_Position(motor_id[0], 3.14, 0.5, 0.5);	//正转1圈
//	CyberGear_Turn_Certain_Position(motor_id[1], -3.14, 0.5, 0.5);	//反转1圈
//	CyberGear_Turn_Certain_Position(motor_id[2], 3.14, 0.5, 0.5);	//正转1圈
//	CyberGear_Turn_Certain_Position(motor_id[3], -3.14, 0.5, 0.5);	//反转1圈
//	delay_ms(5000);
//	
//	CyberGear_Turn_Certain_Position(motor_id[0], -3.14, 0.5, 0.5);	//反转1圈
//	CyberGear_Turn_Certain_Position(motor_id[1], 3.14, 0.5, 0.5);	//正转1圈
//	CyberGear_Turn_Certain_Position(motor_id[2], -3.14, 0.5, 0.5);	//反转1圈
//	CyberGear_Turn_Certain_Position(motor_id[3], 3.14, 0.5, 0.5);	//正转1圈

//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 5, 0.5);	//5rad/s正转
//	CyberGear_Change_Speed(motor_id[1], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[2], 5, 0.5);	//5rad/s正转
//	CyberGear_Change_Speed(motor_id[3], -5, 0.5);	//-5rad/s反转
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 10, 0.5);	//10rad/s正转
//	CyberGear_Change_Speed(motor_id[1], -10, 0.5);	//-10rad/s反转
//	CyberGear_Change_Speed(motor_id[2], 10, 0.5);	//10rad/s正转
//	CyberGear_Change_Speed(motor_id[3], -10, 0.5);	//-10rad/s反转
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 5, 0.5);	//5rad/s正转
//	CyberGear_Change_Speed(motor_id[1], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[2], 5, 0.5);	//5rad/s正转
//	CyberGear_Change_Speed(motor_id[3], -5, 0.5);	//-5rad/s反转
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	//0rad/s正转
//	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	//0rad/s反转
//	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	//0rad/s正转
//	CyberGear_Change_Speed(motor_id[3], 0, 0.5);	//0rad/s反转
//	delay_ms(200);
//	CyberGear_Change_Speed(motor_id[0], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[1], 5, 0.5);	//5rad/s正转
//	CyberGear_Change_Speed(motor_id[2], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[3], 5, 0.5);	//5rad/s正转
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], -10, 0.5);	//-10rad/s反转
//	CyberGear_Change_Speed(motor_id[1], 10, 0.5);	//-10rad/s正转
//	CyberGear_Change_Speed(motor_id[2], -10, 0.5);	//-10rad/s反转
//	CyberGear_Change_Speed(motor_id[3], 10, 0.5);	//-10rad/s正转
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[1], 5, 0.5);	//-5rad/s正转
//	CyberGear_Change_Speed(motor_id[2], -5, 0.5);	//-5rad/s反转
//	CyberGear_Change_Speed(motor_id[3], 5, 0.5);	//-5rad/s正转
//	
//	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[3], 0, 0.5);	//0rad/s
//	
//	delay_ms(5000);
//	CyberGear_Stop_Run(motor_id[0], master_id);		//停转
//	CyberGear_Stop_Run(motor_id[1], master_id);		//停转
//	CyberGear_Stop_Run(motor_id[2], master_id);		//停转
//	CyberGear_Stop_Run(motor_id[3], master_id);		//停转
	
	
	int cnt = 0;
	while(1)
	{   
		

//		DPRINTF("AX:%d\r\n",AX);
		//CyberGear_Control_Mode(motor_id, 1.0, 0.0, 0.0, 0.0, 0.0);
//		DPRINTF("cnt = %d\r\n", cnt);
//		delay_ms(2000);
//		cnt++;
		
	}
}

#else

TaskHandle_t app_task1_handle = NULL;
TaskHandle_t app_task2_handle = NULL;
static void app_task1(void* pvParameters);
static void app_task2(void* pvParameters);

int main(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	//SysTick_Config(SystemCoreClock/configTICK_RATE_HZ);	//开启导致硬件错误中断
	USART1_Init(115200);
	printf("starting...\r\n");
	/* 创建app_task1任务 */
	xTaskCreate((TaskFunction_t )app_task1,  		/* 任务入口函数 */
			  (const char*    )"app_task1",			/* 任务名字 */
			  (uint16_t       )512,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&app_task1_handle);	/* 任务控制块指针 */ 
	
	/* 创建app_task2任务 */		  
	xTaskCreate((TaskFunction_t )app_task2,  		/* 任务入口函数 */
			  (const char*    )"app_task2",			/* 任务名字 */
			  (uint16_t       )512,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&app_task2_handle);	/* 任务控制块指针 */ 
	/* 开启任务调度 */
	vTaskStartScheduler(); 
}

static void app_task1(void* pvParameters)
{
	int cnt = 0;
	for(;;)
	{
		printf("app_task1 is running %d...\r\n", cnt);
		vTaskDelay(1000);
		cnt++;
	}
}

static void app_task2(void* pvParameters)
{
	int cnt = 0;
	for(;;)
	{
		printf("app_task2 is running %d...\r\n", cnt);
		vTaskDelay(1000);
		cnt++;
	}
}

#endif



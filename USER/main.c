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
  * @brief  С�׵������������
  * @param  None
  * @retval None
  */
 


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // ����ϵͳ�ж����ȼ�����2
	USART1_Init(115200);
	DPRINTF("CyberGear run test...\r\n");
	DPRINTF("FUNCTION:%s\tLINE:%d\r\n", __FUNCTION__, __LINE__);
	
	
//	CyberGear_Init();
	
//	CyberGear_Turn_Certain_Position(motor_id[0], 0.3, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[1], 0.3, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[2], 0.3, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[3], 0.3, 0.5, 0.5);	//��ת1Ȧ
//	
//	delay_ms(200);
//	CyberGear_Turn_Certain_Position(motor_id[0], 3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[1], -3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[2], 3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[3], -3.14, 0.5, 0.5);	//��ת1Ȧ
//	delay_ms(5000);
//	
//	CyberGear_Turn_Certain_Position(motor_id[0], -3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[1], 3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[2], -3.14, 0.5, 0.5);	//��ת1Ȧ
//	CyberGear_Turn_Certain_Position(motor_id[3], 3.14, 0.5, 0.5);	//��ת1Ȧ

//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 5, 0.5);	//5rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], 5, 0.5);	//5rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], -5, 0.5);	//-5rad/s��ת
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 10, 0.5);	//10rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], -10, 0.5);	//-10rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], 10, 0.5);	//10rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], -10, 0.5);	//-10rad/s��ת
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 5, 0.5);	//5rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], 5, 0.5);	//5rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], -5, 0.5);	//-5rad/s��ת
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	//0rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	//0rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	//0rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], 0, 0.5);	//0rad/s��ת
//	delay_ms(200);
//	CyberGear_Change_Speed(motor_id[0], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], 5, 0.5);	//5rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], 5, 0.5);	//5rad/s��ת
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], -10, 0.5);	//-10rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], 10, 0.5);	//-10rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], -10, 0.5);	//-10rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], 10, 0.5);	//-10rad/s��ת
//	delay_ms(5000);
//	CyberGear_Change_Speed(motor_id[0], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[1], 5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[2], -5, 0.5);	//-5rad/s��ת
//	CyberGear_Change_Speed(motor_id[3], 5, 0.5);	//-5rad/s��ת
//	
//	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	//0rad/s
//	CyberGear_Change_Speed(motor_id[3], 0, 0.5);	//0rad/s
//	
//	delay_ms(5000);
//	CyberGear_Stop_Run(motor_id[0], master_id);		//ͣת
//	CyberGear_Stop_Run(motor_id[1], master_id);		//ͣת
//	CyberGear_Stop_Run(motor_id[2], master_id);		//ͣת
//	CyberGear_Stop_Run(motor_id[3], master_id);		//ͣת
	
	
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
	//SysTick_Config(SystemCoreClock/configTICK_RATE_HZ);	//��������Ӳ�������ж�
	USART1_Init(115200);
	printf("starting...\r\n");
	/* ����app_task1���� */
	xTaskCreate((TaskFunction_t )app_task1,  		/* ������ں��� */
			  (const char*    )"app_task1",			/* �������� */
			  (uint16_t       )512,  				/* ����ջ��С */
			  (void*          )NULL,				/* ������ں������� */
			  (UBaseType_t    )2, 					/* ��������ȼ� */
			  (TaskHandle_t*  )&app_task1_handle);	/* ������ƿ�ָ�� */ 
	
	/* ����app_task2���� */		  
	xTaskCreate((TaskFunction_t )app_task2,  		/* ������ں��� */
			  (const char*    )"app_task2",			/* �������� */
			  (uint16_t       )512,  				/* ����ջ��С */
			  (void*          )NULL,				/* ������ں������� */
			  (UBaseType_t    )2, 					/* ��������ȼ� */
			  (TaskHandle_t*  )&app_task2_handle);	/* ������ƿ�ָ�� */ 
	/* ����������� */
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



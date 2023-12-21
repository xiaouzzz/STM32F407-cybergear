#include "mecanum.h"
#include "cybergear.h"
#include <math.h>
#include "MPU6050.h"

void forward(float speed){
	CyberGear_Change_Speed(motor_id[0],speed , 0.5);	
	CyberGear_Change_Speed(motor_id[1],speed, 0.5);	
	CyberGear_Change_Speed(motor_id[2],speed, 0.5);	
	CyberGear_Change_Speed(motor_id[3],speed, 0.5);	
}
void back(void){
	CyberGear_Change_Speed(motor_id[0],-5 , 0.5);	
	CyberGear_Change_Speed(motor_id[1],-5, 0.5);	
	CyberGear_Change_Speed(motor_id[2],-5, 0.5);	
	CyberGear_Change_Speed(motor_id[3],-5, 0.5);	
}
void left(void){
	CyberGear_Change_Speed(motor_id[0],5 , 0.5);	
	CyberGear_Change_Speed(motor_id[1],-5, 0.5);	
	CyberGear_Change_Speed(motor_id[2],5, 0.5);	
	CyberGear_Change_Speed(motor_id[3],-5, 0.5);	
}
void right(void){
	CyberGear_Change_Speed(motor_id[0],-5 , 0.5);	
	CyberGear_Change_Speed(motor_id[1],5, 0.5);	
	CyberGear_Change_Speed(motor_id[2],-5, 0.5);	
	CyberGear_Change_Speed(motor_id[3],5, 0.5);	
}

void brake(void){
	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	
	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	
	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	
	CyberGear_Change_Speed(motor_id[3], 0, 0.5);	
}


void stop(void){
	CyberGear_Stop_Run(motor_id[0], master_id);		//ͣת
	CyberGear_Stop_Run(motor_id[1], master_id);		//ͣת
	CyberGear_Stop_Run(motor_id[2], master_id);		//ͣת
	CyberGear_Stop_Run(motor_id[3], master_id);		//ͣת
}

// ȫ���ƶ�
void Move_Transfrom(float tar_x,float tar_y)
{   
	
	
	
//	int currentX = readXPosition();
//    int currentY = readYPosition();
//	int currentZ = readZPosition();
//	
//	//�����ƶ�����;���
//    double distance = sqrt(pow((tar_x - currentX), 2) + pow((tar_y - currentY), 2));
//    double theta = atan2(tar_y, tar_x);  // �ƶ�����ĽǶȣ������ƣ�
//    double rad = theta * 180.0 / 3.14;  // ���ȶ�Ӧ�ĽǶ�
//	
//	// ����ÿ�������ķ�ֵ�������ٶ�Ϊ5��λ/��
//    double linear_x = 5 * cos(theta);
//    double linear_y = 5 * sin(theta);
//	double angular_z = 0;
//	
//	if(distance>5){
//	CyberGear_Change_Speed(motor_id[0], linear_x - linear_y + angular_z, 0.5);
//	CyberGear_Change_Speed(motor_id[1], linear_x + linear_y + angular_z, 0.5);	
//	CyberGear_Change_Speed(motor_id[2], linear_x + linear_y - angular_z, 0.5);	
//	CyberGear_Change_Speed(motor_id[3], linear_x - linear_y - angular_z, 0.5);
//	}
//	else{
//	CyberGear_Change_Speed(motor_id[0], 0, 0.5);	
//	CyberGear_Change_Speed(motor_id[1], 0, 0.5);	
//	CyberGear_Change_Speed(motor_id[2], 0, 0.5);	
//	CyberGear_Change_Speed(motor_id[3], 0, 0.5);
//	}

}

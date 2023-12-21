#ifndef __MECANUM_H
#define __MECANUM_H
	
#include "cybergear.h"


void forward(float speed);
void back(void);
void left(void);
void right(void);
void brake(void);
void stop(void);
void Move_Transfrom(float tar_x,float tar_y);
	
	
#endif


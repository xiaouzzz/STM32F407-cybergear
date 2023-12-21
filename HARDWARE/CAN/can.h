#ifndef __CAN_H
#define __CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"

extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;


void CAN1_Init(void);
int8_t CAN1_SendData(uint8_t data[], uint8_t len, uint32_t msgId);
void CAN1_RX0_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif


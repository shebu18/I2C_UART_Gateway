/**
 * USART_lib.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Cristian Gantner
 */


#include "stm32f10x_conf.h"


#ifndef USART_LIB_H_
#define USART_LIB_H_


#define USART_BAUD 	256000
#define USART		 	USART1
#define USART_PORT 	GPIOA
#define USART_TX_PIN	GPIO_Pin_9
#define USART_RX_PIN	GPIO_Pin_10

#define BUFFER_SIZE 512
#define DMA_BUFFER_SIZE 128

extern u16 Write;
extern u8 Read;
extern u8 newData;
extern u8 RecieveBuffer[];
extern char SendBuffer[];
extern u8 recieveCount;


void USART_init(void);
void USART_sendChar(char data);
void USART_sendString(char *str);
void USART_receive();
void USART_DMA_Send(u16 len);//send content of SendBuffer

#endif /* USART_LIB_H_ */

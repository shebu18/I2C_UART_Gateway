/**
 * @file I2C_lib.h
 *
 *  Created on: Mar 8, 2017
 * @author  Cristian Gantner
 */



#ifndef I2C_LIB_H_
#define I2C_LIB_H_

#include "stm32f10x_conf.h"

#define I2C_CHAN		I2C1
#define I2C_CLK		RCC_APB1Periph_I2C1
#define I2C_PORT_CLK 	RCC_APB2Periph_GPIOB
#define I2C_PORT		GPIOB
#define I2C_SCL		GPIO_Pin_8
#define I2C_SDA		GPIO_Pin_9
#define I2C_SPD		400000

#define I2C_BUFFER_SIZE 128

extern uint8_t i2cData[I2C_BUFFER_SIZE];
extern uint16_t i2cDataCount;


void Init_I2C();

int I2C_gatew_send();

int I2C_gatew_receive(uint16_t readDatan);

int I2C_Start(uint8_t address,uint8_t direction);

int I2C_Stop(void);

int I2C_Send(uint8_t data);

int I2C_Receive_ACK(void);

#endif /* I2C_LIB_H_ */

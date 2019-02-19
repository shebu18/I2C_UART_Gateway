/**
 * @file  I2C_lib.c
 *
 *  Created on: Mar 8, 2017
 * @author Cristian Gantner
 */


#include <I2C_lib.h>
#include "USART_lib.h"
#include "stdio.h"

uint8_t i2cData[I2C_BUFFER_SIZE];
uint16_t i2cDataCount = 0;

#define TIMEOUT 10000;

char strBuff[64]; //buffer for conversion form int to string

/**
 * @brief Function initializes I2C at defined pins.
 *
 * @param None
 * @return None
 */
void Init_I2C (void){

	RCC_APB2PeriphClockCmd(I2C_PORT_CLK|RCC_APB2Periph_AFIO, ENABLE);		//enable GPIOB and AF clock
	RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);								//enable I2C clock

	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE); //Remaping I2C1 to PB8 and PB9

	/* Configure the GPIOs used to drive the I2C LCD*/
	GPIO_InitStructure.GPIO_Pin = I2C_SDA|I2C_SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C_PORT, &GPIO_InitStructure);

	I2C_DeInit(I2C_CHAN); //reset I2C registers to default value
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed=I2C_SPD;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1=0;
	I2C_Init(I2C_CHAN, &I2C_InitStructure);
	I2C_Cmd(I2C_CHAN, ENABLE);
}

/**
 * @brief Function start I2C transaction and send bytes to slave.
 *
 * @param None
 * @return None
 */
int I2C_gatew_send(){
	uint16_t x;

	if(I2C_Start(i2cData[0], I2C_Direction_Transmitter)) {return 1;}

	for(x=1; x<=(i2cDataCount - 1); x++){
		if(I2C_Send(i2cData[x])) {return 1;}
		if(i2cData[x]!=0){
		sprintf(strBuff, "Sending value: %#02X\r", i2cData[x]);
		USART_sendString(strBuff);
		}else{
			sprintf(strBuff, "Sending value: 0x%#02X\r", i2cData[x]);
			USART_sendString(strBuff);
		}

	}
	I2C_Stop();
	i2cDataCount = 0;

	return 0;
}

/**
 * @brief Function start I2C transaction and reads bytes from slave.
 *
 * @param readDatan Number of bytes to be read from slave
 * @return None
 */
int I2C_gatew_receive(uint16_t readDatan){

	uint16_t x=0;
	if(I2C_Start(i2cData[0], I2C_Direction_Receiver)) {return 1;}

	if(readDatan ==2 ){
		//i2cData[x] = I2C_Receive_ACK();
		//x++;
		I2C_AcknowledgeConfig(I2C_CHAN, ENABLE);
		while(!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_RECEIVED));
		i2cData[x]=I2C_ReceiveData(I2C_CHAN);
		x++;

		I2C_AcknowledgeConfig(I2C_CHAN, DISABLE);
		I2C_GenerateSTOP(I2C_CHAN, ENABLE);
		// read last byte and generate NAK(errata 2.13)
		while(!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_RECEIVED));
		//I2C_GenerateSTOP(I2C_CHAN, ENABLE);
		i2cData[x]=I2C_ReceiveData(I2C_CHAN);


		//print to console received bytes as requested
		USART_sendString("Received: ");
		for (x = 0; x < readDatan; x++) {
			if(i2cData[x]!=0){
				sprintf(strBuff, "%#02X\r", i2cData[x]);
				USART_sendString(strBuff);
			}else{
				sprintf(strBuff, "0x%#02X\r", i2cData[x]);
				USART_sendString(strBuff);
			}
		}

		i2cDataCount = 0;
		return 0;
	}
	for(x=0; x<(readDatan-2); x++){
		i2cData[x] = I2C_Receive_ACK();
		}

	// read pre-last byte and prepare for NACK(read errata 2.13)
	while(!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_RECEIVED));

	I2C_AcknowledgeConfig(I2C_CHAN, DISABLE);
	i2cData[x]=I2C_ReceiveData(I2C_CHAN);
	x++;
	I2C_GenerateSTOP(I2C_CHAN, ENABLE);

	// read last byte and generate NAK(errata 2.13)
	while( !I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_RECEIVED));
	//I2C_GenerateSTOP(I2C_CHAN, ENABLE);
	i2cData[x]=I2C_ReceiveData(I2C_CHAN);

	//print to console received bytes as requested
	USART_sendString("Received: ");
	for (x = 0; x < readDatan; x++) {
		if(i2cData[x]!=0){
			sprintf(strBuff, "%#02X\r", i2cData[x]);
			USART_sendString(strBuff);
		}else{
			sprintf(strBuff, "0x%#02X\r", i2cData[x]);
			USART_sendString(strBuff);
		}
	}

	i2cDataCount = 0;
	return 0;
}

/**
 * @brief Function starts I2C transaction.
 *
 * @param address Address of I2C Slave device
 * @param direction specifies whether the I2C device will be a
  *   Transmitter or a Receiver. This parameter can be one of the following values
  *     @arg I2C_Direction_Transmitter: Transmitter mode
  *     @arg I2C_Direction_Receiver: Receiver mode
 * @return None
 */
int I2C_Start(uint8_t address, uint8_t direction) {
    uint16_t x = TIMEOUT;
    uint8_t status = 0;

    // wait until I2C1 is not busy any more
    while (((I2C_GetFlagStatus(I2C_CHAN, I2C_FLAG_BUSY)) & (x != 0))) {
    	x--;
    	if(x==0){
    		status = 1;
    	} else status = 0;
    };

    //generate I2C Start signal
    I2C_GenerateSTART(I2C_CHAN, ENABLE);

    x = TIMEOUT;
    //wait until master mode is selected
    while (((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_MODE_SELECT)) & (x != 0))) {
    	x--;
    	if(x==0){
    		status = 1;
    	} else status = 0;
    };

    //send 7 bit address
    I2C_Send7bitAddress(I2C_CHAN, address, direction);

    x = TIMEOUT;
    //wait until master mode or receiver mode is selected
    if (direction == I2C_Direction_Transmitter) {
    	while (((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) & (x != 0))) {
    		x--;
    		if(x==0){
    			status = 1;
    		} else status = 0;
    	};
    } else if (direction == I2C_Direction_Receiver) {
    	while (((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) & (x != 0))) {
    		x--;
    		if(x==0){
    			status = 1;
    		} else status = 0;
    	};
    }

    if (status == 1) {
    	USART_sendString("Address not acknowledged or wrong address. Check alignment.");
    	return 1;
    }

    return 0;
}

/**
 * @brief Function stops I2C transaction.
 *
 * @param None
 * @return None
 */
int I2C_Stop(void){
    uint16_t x = TIMEOUT;
    uint8_t status = 0;

    while(((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) & (x != 0))) {
		x--;
		if(x==0){
			status = 1;
		} else status = 0;
	};

	// Send I2C1 STOP Condition after last byte has been transmitted
	I2C_GenerateSTOP(I2C_CHAN, ENABLE);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) & (x != 0))) {
		x--;
		if(x==0){
			status = 1;
		} else status = 0;
	}

    if (status == 1) {
    	USART_sendString("Error sending STOP.");
    	return 1;
    }

    return 0;
}

/**
 * @brief Function sends byte over I2C.
 *
 * @param data Data to be send over I2C
 * @return None
 */
int I2C_Send(uint8_t data){
    uint16_t x = TIMEOUT;
    uint8_t status = 0;

	//send byte to OLED
	I2C_SendData(I2C_CHAN, data);
	//wait until byte is transmitted
	while(((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) & (x != 0))) {
		x--;
		if(x==0){
			status = 1;
		} else status = 0;
	}

    if (status == 1) {
    	USART_sendString("Error sending Data.");
    	return 1;
    }

    return 0;
}

/**
 * @brief Function receives byte over I2C with acknowledge.
 *
 * @param None
 * @return received data
 */
int I2C_Receive_ACK(){
    uint16_t x = TIMEOUT;
    uint8_t status = 0;

	// enable acknowledge of received data
	I2C_AcknowledgeConfig(I2C_CHAN, ENABLE);
	// wait until one byte has been received
	while(((!I2C_CheckEvent(I2C_CHAN, I2C_EVENT_MASTER_BYTE_RECEIVED)) & (x != 0))) {
		x--;
		if(x==0){
			status = 1;
		} else status = 0;
	}

	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2C_CHAN);

    if (status == 1) {
    	USART_sendString("Error sending Data.");
    	return 1;
    }

	return data;
}

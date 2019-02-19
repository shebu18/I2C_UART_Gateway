/**
 * @file USART_lib.c
 *
 *  Created on: Oct 8, 2017
 * @author Cristian Gantner
 */


#include <USART_lib.h>
#include "string.h"

u8 RecieveBuffer[BUFFER_SIZE];
char SendBuffer[BUFFER_SIZE];
u8 DMA_RECIEVE_BUFFER[DMA_BUFFER_SIZE];
u8 newData = 0;
u8 recieveCount = 0;
u16 Write = 0;
u8 Read = 0;


void USART_init(void){
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//enable GPIOA clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);						//enable USART1 Clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//USART pin setup
	GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);

	//USART setup
	USART_InitStructure.USART_BaudRate = USART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART, &USART_InitStructure);

	//interrupt setup
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA1 Channel5 setup for receiving data over USART
	DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART->DR);
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DMA_RECIEVE_BUFFER[0];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	/* Enable global interrupts for DMA1 channel 5 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);



	USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE);


	USART_ITConfig(USART, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART, ENABLE);

	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);

}


void USART_sendChar(char data){
	USART_SendData(USART, data);
	while(!USART_GetFlagStatus(USART, USART_FLAG_TXE));
}

void USART_sendString(char *str){

	while(*str){
		USART_sendChar(*str++);
	}
}

void USART_receive(){


}

void USART_DMA_Send(u16 len){
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//u16 a = sizeof(SendBuffer);
	//DMA1 Channel4 setup for sending data over USART
	DMA_InitStructure.DMA_BufferSize = len; //(uint16_t)(sizeof(SendBuffer)/sizeof(SendBuffer[0]));
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART->DR);
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&SendBuffer[0];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

	/* Enable global interrupts for DMA1 channel 4 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART, USART_DMAReq_Tx, ENABLE);

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

extern void USART1_IRQHandler(){
	u16 len =0, tocopy = 0;
	u8* ptr;

	/* Check for IDLE flag */
	if (USART->SR & USART_FLAG_IDLE) {         /* We want IDLE flag only */
		/* This part is important */
		/* Clear IDLE flag by reading status register first */
		/* And follow by reading data register */
		volatile uint32_t tmp;                  /* Must be volatile to prevent optimizations */
		tmp = USART->SR;                       /* Read status register */
		tmp = USART->DR;                       /* Read data register */
		(void)tmp;                              /* Prevent compiler warnings */
		DMA_Cmd(DMA1_Channel5, DISABLE);       /* Disabling DMA will force transfer complete interrupt if enabled */

		/* Calculate number of bytes actually transfered by DMA so far */
		/**
		 * Transfer could be completed by 2 events:
		 *  - All data actually transfered (NDTR = 0)
		 *  - Stream disabled inside USART IDLE line detected interrupt (NDTR != 0)
		 */
		len = DMA_BUFFER_SIZE - DMA1_Channel5->CNDTR;
		tocopy = BUFFER_SIZE - Write;      /* Get number of bytes we can copy to the end of buffer */

		/* Check how many bytes to copy */
		if (tocopy > len) {
			tocopy = len;
		}

		/* Write received data for UART main buffer for manipulation later */
		ptr = DMA_RECIEVE_BUFFER;
		memcpy(&RecieveBuffer[Write], ptr, tocopy);   /* Copy first part */

		/* Correct values for remaining data */
		Write += tocopy;
		len -= tocopy;
		ptr += tocopy;

		/* If still data to write for beginning of buffer */
		if (len) {
			memcpy(&RecieveBuffer[0], ptr, len);      /* Don't care if we override Read pointer now */
			Write = len;
		}

		/* Prepare DMA for next transfer */
		/* Important! DMA stream won't start if all flags are not cleared first */
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_GL5);
		DMA1_Channel5->CMAR = (uint32_t)&DMA_RECIEVE_BUFFER[0];   /* Set memory address for DMA again */
		DMA1_Channel5->CNDTR = DMA_BUFFER_SIZE;    /* Set number of bytes to receive */
		DMA_Cmd(DMA1_Channel5, ENABLE);            /* Start DMA transfer */
	}

	newData = 1;


	/*uint8_t tmp;

	while(USART_GetITStatus(USART, USART_IT_RXNE) != RESET){
			RecieveBuffer[recieveCount++]= USART_ReceiveData(USART);
	}
	if((recieveCount > BUFFER_SIZE)){
		USART_sendChar('6');
		USART_ITConfig(USART, USART_IT_RXNE, DISABLE);
		//return;
	}
	while(USART_GetITStatus(USART, USART_IT_IDLE) != RESET){
		tmp = USART->SR;
		tmp = USART->DR;
		(void)tmp;
		newData = 1;}*/
}

extern void DMA1_Channel5_IRQHandler(){
	u16 len =0, tocopy = 0;
	u8* ptr;

	/* Check transfer complete flag */
	if (DMA_GetFlagStatus(DMA1_FLAG_TC5)) {
		DMA_ClearFlag(DMA1_FLAG_TC5);           /* Clear transfer complete flag */

		/* Calculate number of bytes actually transfered by DMA so far */
		/**
		 * Transfer could be completed by 2 events:
		 *  - All data actually transfered (NDTR = 0)
		 *  - Stream disabled inside USART IDLE line detected interrupt (NDTR != 0)
		 */
		len = DMA_BUFFER_SIZE - DMA1_Channel5->CNDTR;
		tocopy = BUFFER_SIZE - Write;      /* Get number of bytes we can copy to the end of buffer */

		/* Check how many bytes to copy */
		if (tocopy > len) {
			tocopy = len;
		}

		/* Write received data for UART main buffer for manipulation later */
		ptr = DMA_RECIEVE_BUFFER;
		memcpy(&RecieveBuffer[Write], ptr, tocopy);   /* Copy first part */

		/* Correct values for remaining data */
		Write += tocopy;
		len -= tocopy;
		ptr += tocopy;

		/* If still data to write for beginning of buffer */
		if (len) {
			memcpy(&RecieveBuffer[0], ptr, len);      /* Don't care if we override Read pointer now */
			Write = len;
		}

		/* Prepare DMA for next transfer */
		/* Important! DMA stream won't start if all flags are not cleared first */
		DMA_Cmd(DMA1_Channel5, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_GL5);
		DMA1_Channel5->CMAR = (uint32_t)&DMA_RECIEVE_BUFFER[0];   /* Set memory address for DMA again */
		DMA1_Channel5->CNDTR = DMA_BUFFER_SIZE;    /* Set number of bytes to receive */
		DMA_Cmd(DMA1_Channel5, ENABLE);            /* Start DMA transfer */
	}
}

extern void DMA1_Channel4_IRQHandler(){

	if (DMA_GetFlagStatus(DMA1_FLAG_TC4)) {
		DMA_ClearFlag(DMA1_FLAG_TC4);
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_GL5);
	}


}

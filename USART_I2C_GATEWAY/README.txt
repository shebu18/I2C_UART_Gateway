[INITIAL]

- this program is designed for the STM32F103 micro controller family
- this program uses I2C1
- pins PB8-SCL and PB9-SDA are used
- I2C speed is set to 400kHz, this can be changed in the I2C_lib.h file by changing I2C_SPD define

[USAGE]

- the string is configured as follows
	- "[" is the start character and needs to be send
	- after the start character the address follows in the format 0xAB (IMPORTANT USE CAPITAL LETTERS), no space required before address
		use left aligned write and read address
	- following data bytes are separated bye SPACE character
	- max number of characters is 512 (size of receiveBUFFER in USART_lib.h)
	- max data values for transfer is 128 (size of i2cDATA BUFFER in I2C_lib.h)
	- every transaction is terminated by the "]" character
	- for reading data from the slave send the address followed by r(read command) and number of bytes to be read(0-128)
	
	EXAMPLE: WRITE [0xA0 0xCD 0xDF 0xC8] - this will send to the slave with address 0xA0 the data 0xCD 0xDF 0xC8
			 READ  [0xA1 r5] - this will read from slave with address 0xA1 5 bytes of data and send them over USART
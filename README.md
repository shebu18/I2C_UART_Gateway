I2C-USART Gateway on the STM32F103C8

**[INITIAL]**

- this application was created for a lab i took during my master 
- it is inspired by the [BusPirate I2C](http://dangerousprototypes.com/blog/bus-pirate-manual/i2c-guide/) communication
- this program is designed for the STM32F103 micro controller family
- this program uses I2C1
- pins PB8-SCL and PB9-SDA are used
- I2C speed is set to 400kHz, this can be changed in the I2C_lib.h file by changing I2C_SPD define
- USART speed is set to 256000. Can be changed in USART_lib.h file by changing USART_BAUD define
- use no or one end of line character

**[USAGE]**

- the string is configured as follows
- "\[" is the start character and needs to be send
- after the start character the address follows in the format 0xAB (IMPORTANT USE CAPITAL LETTERS), no space required before address
	use left aligned write and read address
- following data bytes are separated by the SPACE character
- max number of characters is 512 (size of receiveBUFFER in USART_lib.h)
- max data values for transfer is 128 (size of i2cDATA BUFFER in I2C_lib.h)
- every transaction is terminated by the "\]" character
- for reading data from the slave send the address followed by r(read command) and number of bytes to be read(0-128)
- size of buffers can be adjusted accordingly 
	
**EXAMPLE:** 
- WRITE [0xA0 0xCD 0xDF 0xC8] - this will send to the slave with address 0xA0 the data 0xCD 0xDF 0xC8
- READ  [0xA1 r5] - this will read from slave with address 0xA1 5 bytes of data and send them over USART

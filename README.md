Open source Satellite Initiative
http://opensat.cc

Testing USART I2c on MSP430F1611

I2C Master
- IFG polling to report status and error for I2C communication
- Timeout occurs when error is detected using Timer A
- General Call Address Send
- Ack Polling
- Low Power Mode 3

I2C Slave
- Interrupt based 
- Byte addressable buffer
- General call receive
- Low Power Mode 3


Comment
- Using USART I2C on MSP430F1611 is hard: Not intuitive at all
- Would use USCI I2C 




#ifndef I2C_H
#define I2C_H

#include "includes.h"

#define I2C_DDR			DDRC
#define I2C_PORT		PORTC	

#define SDA 			PC6
#define SCL 			PC5

#define I2C_DELAY 		0xFFFF	

void I2C_delay(void);
void I2C_clock(void);
void I2C_start(void);
void I2C_stop(void);
unsigned char I2C_write(unsigned char dat);
unsigned char I2C_read(void);
void I2C_ack(void);
void I2C_noack(void);

#endif

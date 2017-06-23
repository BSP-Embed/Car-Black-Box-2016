#include"i2c.h"

void I2C_delay(void)
{
	_delay_us(1);
}

void I2C_clock(void)
{
	I2C_delay();
    I2C_PORT |=  _BV(SCL); 
								//SCL = 1;		/* Start clock */
	I2C_delay();    
	I2C_PORT &= ~ _BV(SCL);
								//SCL = 0;		/* Clear SCL */
}

void I2C_start(void)
{
	I2C_DDR |= _BV(SCL) | _BV(SDA);			
								// SDA ACTS AS OUTPUT
	
	I2C_PORT |=  _BV(SDA); 		//SDA = 1;        /* Set SDA */
	I2C_PORT |=  _BV(SCL); 		//SCL = 1;		/* Set SCL */

	I2C_delay(); 

	I2C_PORT &= ~ _BV(SDA); 	//SDA = 0;        /* Clear SDA */

	I2C_delay(); 

	I2C_PORT &= ~ _BV(SCL); 	//SCL = 0;        /* Clear SCL */
}

void I2C_stop(void)
{
	I2C_DDR |= _BV(SDA);		// SDA ACTS AS OUTPUT

	I2C_PORT |=  _BV(SCL); 		//SCL = 1;			/* Set SCL */
	I2C_delay();

	I2C_PORT &= ~ _BV(SDA); 	//SDA = 0;			/* Clear SDA */

	I2C_delay();

	I2C_PORT |=  _BV(SDA); 		//SDA = 1;			/* Set SDA */
}

unsigned char I2C_write(unsigned char dat)
{
	unsigned char data_bit;		
	unsigned char i;	
	I2C_DDR |= _BV(SDA);		// SDA ACTS AS OUTPUT

	for (i = 0; i < 8; i++)	{			/* For loop 8 time(send data 1 byte) */
	
		
		if( dat & 0x80 )
			I2C_PORT |=  _BV(SDA);
		else
			I2C_PORT &=  ~_BV(SDA);				/* Send data_bit to SDA */

		I2C_clock();      			/* Call for send data to i2c bus */

		dat = dat<<1;  
	}
	I2C_PORT &=  ~_BV(SDA); //SDA = 1;			/* Set SDA */

	I2C_delay();	
	
	I2C_PORT |=  _BV(SCL);  //SCL = 1;			/* Set SCL */

	I2C_delay();	

	I2C_DDR &= ~_BV(SDA);		// SDA ACTS AS INPUT

	if ( bit_is_set(PINC,SDA) )		/* Check acknowledge */
			data_bit = 1;
		else
			data_bit = 0;

	I2C_PORT &= ~ _BV(SCL); 	//SCL = 0;			/* Clear SCL */



	return data_bit;			/* If send_bit = 0 i2c is valid */		 	
}

unsigned char I2C_read(void)
{
	unsigned char rd_bit;	
	unsigned char i, dat;
	dat = 0x00;	
	I2C_DDR &= ~_BV(SDA);		// SDA ACTS AS INPUT

	for(i=0;i<8;i++)		/* For loop read data 1 byte */
	{
		I2C_delay();

        I2C_PORT |=  _BV(SCL);		//SCL = 1;			/* Set SCL */

		I2C_delay(); 

		if ( bit_is_set(PINC,SDA) )
			rd_bit = 1;
		else
			rd_bit = 0;

		dat = dat<<1;		
		dat = dat | rd_bit;	/* Keep bit data in dat */

        I2C_PORT &= ~ _BV(SCL);		//SCL = 0;			/* Clear SCL */
	}


	return dat;
}

void I2C_ack(void)
{
    I2C_PORT &= ~ _BV(SDA);		//	SDA = 0;		/* Clear SDA */

	I2C_delay();    

	I2C_clock();					/* Call for send data to i2c bus */

	I2C_PORT |=  _BV(SDA);  		//SDA = 1;		/* Set SDA */
}

void I2C_noack(void)
{
    I2C_PORT |=  _BV(SDA);	//SDA = 1;		/* Set SDA */

	I2C_delay();

	I2C_clock();	/* Call for send data to i2c bus */

    I2C_PORT |=  _BV(SCL);//	SCL = 1;		/* Set SCL */
}
							









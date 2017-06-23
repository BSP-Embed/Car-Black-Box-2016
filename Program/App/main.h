#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

#define	ULTSEN_DDR			DDRB
#define	ULTSEN_PORT			PORTB
#define	ULTSEN_PIN			PINB
#define ECHO_PIN			PB1
#define TRIG_PIN			PB0

#define INTDDR				DDRD
#define INTPORT				PORTD
#define INT0_PIN			PD2
#define INT1_PIN			PD3

#define SENSOR_DDR			DDRD
#define SENSOR_PORT			PORTD
#define SENSOR_PIN			PIND
#define DOOR_PIN			PD6

#define ALSENS_DDR			DDRD
#define ALSENS_PORT			PORTD
#define ALSENS_IPIN			PIND
#define ALSENS_PIN			PD4

#define SWITCH_DDR			DDRD
#define SWITCH_PORT			PORTD
#define SWITCH_IOPIN		PIND
#define SWITCH_PIN			PD2

//DEFINE CONSTANT
#define	INFO_EE_ADDR		0x01

#define LM35_MUL_FACTOR		0.01			/* 10mv/degree */
#define STEP_SIZE			0.00488			/* 5v / 1024 */
#define ULTRASONIC_CONST	0.01724
#define THRESHOLD_DIST		1				/* in meters */

#define FRONT				1
#define BACK				2

#define MSG1 				"Collision Occurred Near NH212."

//DEFINE MACROS
#define StartTmr()			TCCR0  	|= _BV(CS01)
#define StopTmr()			TCCR0  	&= ~_BV(CS01)

#define PWMDC(x)			(OCR2 = (x) * 2.55)

//FUNCTION PROTOTYPES
static void		 init		(void);
static void 	disptitl 	(void);
static void 	tmr1init	(void);
static void 	tmr0init	(void);
static float	measdist	(int8u disp);
static float 	meastemp	(int8u disp);
static void		EXT0init	(void);	
static void		inittest	(void);
static void		sensorinit	(void);
static void		movevehi	(void);
static void		stopvehi	(void);
static void 	chksens		(void);
static int16u 	measco2		(int8u disp);
static void 	logparam	(void);
static void 	dispparam	(void);
static void 	displog		(void);
static void 	incspeed	(void);

#endif

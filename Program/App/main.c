#include "main.h"

volatile int8u chkdistflag = 0;
volatile int8u monitflag = 0;

volatile int16u ultpulse;
volatile int8u colliflag = 0;
volatile int8u collisens = 0;
volatile int8u stopflag = 0;
volatile int8u moveflag = 0;

extern lcdptr;

const char *stat[4] = { 
	"Door Opened",
	"Alcohol Detected",
	"Front collision",
	"back collision" 
};

int main(void)
{
	int8u i;
	init();
	
	while (TRUE) {

		if (chkdistflag == 1) {
			if (measdist(1) < THRESHOLD_DIST) {
				beep(3,75);
				if (stopflag == 0) {
					stopflag = 1;
					moveflag = 0;
					lcdclr();
					lcdws(" Object Detected");
					lcdr2();
					lcdws("  Slowing Down");
					for (i = 100; i > 0; i--) {
						PWMDC(i);
						dlyms(20);
					}
					stopvehi();
					dispparam();
				}
			} else {
				if (moveflag == 0) {
					moveflag = 1;
					stopflag = 0;
					movevehi();
					dispparam();
				}
			}
			chkdistflag = 0;
		}

		if (colliflag == 1) {
			buzon();
			colliflag = 0;
			stopvehi();
			lcdclr();
			lcdws("COLLI OCC: ");
			if (collisens == FRONT)
				lcdws("FRONT");
			else
				lcdws("BACK");
			
			logparam();
			dlyms(1000);
			GSMsndmsg(OPhNum, MSG1);
			lcdclr();
			lcdws("Collision Occu'd");
			lcdr2();
			lcdws("Vehicle Stopped!");
			ledon();
			cli();
			sleep_enable();
			sleep_cpu();
			for(;;);	
		}

		if (monitflag == 1) {
			monitflag = 0;
			meastemp(1);
			measco2(1);
			chksens();
		}
	}
	return 0;
}
static void init(void)
{
	buzinit();
	ledinit();

	ULTSEN_DDR 		|= _BV(TRIG_PIN);
	ULTSEN_DDR 		&= ~_BV(ECHO_PIN);

	ULTSEN_PORT		&= ~_BV(TRIG_PIN);
	ULTSEN_PORT		|= _BV(ECHO_PIN);

	SENSOR_DDR 		&= ~_BV(DOOR_PIN);
	SENSOR_PORT		|= _BV(DOOR_PIN);

	ALSENS_DDR		&= ~_BV(ALSENS_PIN);
	ALSENS_PORT		|= _BV(ALSENS_PIN);

	SWITCH_DDR		&= ~_BV(SWITCH_PIN);
	SWITCH_PORT		|= _BV(SWITCH_PIN);
	
	beep(2,100);
	lcdinit();

	if ((SWITCH_IOPIN & _BV(SWITCH_PIN)) == 0)
		displog();
	
	motorinit();
	uartinit();
	tmr0init();
	tmr1init();
	adcinit();
	EXT0init();
	PWMinit();
	GSMinit();
	sensorinit();
	disptitl();
	inittest();
	movevehi();
	dispparam();
	sei();

}
static void incspeed(void)
{
	int8u i;

	for (i = 0; i < 100; i++) {
		PWMDC(i);
		dlyms(10);
	} 
}
static void disptitl(void)
{ 
	lcdclr();
	lcdws(" VEHI BLACK BOX");
	dlyms(1000);
}
static void tmr1init(void)
{
	TCNT1H   = 0xD3;
	TCNT1L   = 0x00;
	TIMSK   |= _BV(TOIE1);			//ENABLE OVERFLOW INTERRUPT
	TCCR1A   = 0x00;					
	TCCR1B  |= _BV(CS10) | _BV(CS11); /* PRESCALAR BY 16 */
}
static void tmr0init(void)
{
	TCNT0   =  167;
	TIMSK   |= _BV(TOIE0);			//ENABLE OVERFLOW INTERRUPT
	
}
static void EXT0init(void)
{
	INTDDR 	&= ~_BV(INT0_PIN);
	INTPORT |= _BV(INT0_PIN);

	INTDDR 	&= ~_BV(INT1_PIN);
	INTPORT |= _BV(INT1_PIN);

	GICR |= _BV(INT0) | _BV(INT1);			//ENABLE EXTERNAL INTERRUPT
	MCUCR |= _BV(ISC01) | _BV(ISC11);		//FALLING EDGE INTERRUPT

}
/* overflows at every 100msec */
ISR(TIMER1_OVF_vect) 
{ 
	static int8u i,j,k;

	TCNT1H = 0xD3;
	TCNT1L = 0x00;
	
	if (++i >= 50) i = 0;
	switch(i) {
		case 0: case 2: ledon();  break;
		case 1: case 3: ledoff(); break;
	} 
	if (++j >= 10) {
		j = 0;
		chkdistflag = 1;
	}
	if (++k >= 20) {
		k = 0;
		monitflag = 1;
	}
}
/* OverFlows every 100us */
ISR(TIMER0_OVF_vect) 
{ 
	TCNT0 = 167;
	++ultpulse;
}
ISR(INT0_vect) 
{ 
	collisens = FRONT;
	colliflag = 1;
	GICR &= ~_BV(INT0);
}
ISR(INT1_vect) 
{ 
	collisens = BACK;
	colliflag = 1;
	GICR &= ~_BV(INT1);
}
static float measdist(int8u disp)
{
	float dist;
	char s[10];

	ULTSEN_PORT		|= _BV(TRIG_PIN);
	dlyus(10);
	ULTSEN_PORT		&= ~_BV(TRIG_PIN);
	ultpulse = 0;

	while ((ULTSEN_PIN & _BV(ECHO_PIN)) == 0);
	StartTmr();
	while (ULTSEN_PIN & _BV(ECHO_PIN));
	StopTmr();
	
	dist = ultpulse * ULTRASONIC_CONST;
	
	if (disp == 1) {
		lcdptr = 0xc9;
		lcdws("    ");
		ftoa(dist,s);
		lcdptr = 0xc9;
		lcdws(s);
	}

	return dist;
}

static void inittest(void)
{
		lcdclr();
		lcdws("Chk Alcohol:");
		if ((ALSENS_IPIN & _BV(ALSENS_PIN)) == 0x00) {
			lcdws("N'OK");
			while ((ALSENS_IPIN & _BV(ALSENS_PIN)) == 0x00) {
				beep(1,75);
				dlyms(500);
			}
		} else {
			lcdws("OK");
			dlyms(1000);
		}
			
	lcdclr();
	lcdws("Chk Doors:");
	
	if (SENSOR_PIN & _BV(DOOR_PIN)) {
		lcdws("NOT OK");
		while (SENSOR_PIN & _BV(DOOR_PIN)) {
			beep(1,75);
			dlyms(500);
		}
	} else {
		lcdws("OK");
		dlyms(1000);
	}
}
static void sensorinit(void)
{
	lcdclr();
	lcdws("ALCOHOL SENS:");
	while ((ALSENS_IPIN & _BV(ALSENS_PIN)) == 0) {
		beep(1,250);
		dlyms(500);
	}
	lcdws("OK");
	dlyms(1000);
}
static void movevehi(void)
{
	lcdclr();
	lcdws(" Vehicle Moving");
	mot1on();
	dlyms(500);
	incspeed();
}
static void stopvehi(void)
{
	lcdclr();
	lcdws("Vehicle Stopped");
	PWMDC(0);
	mot1off();
	dlyms(500);
}
static float meastemp(int8u disp)
{
	float temp;
	char s[10];

	temp = adcget(0);

	temp = temp * STEP_SIZE;
	temp /= LM35_MUL_FACTOR;
	
	if (disp == 1) {
		lcdptr = 0x82;
		lcdws("     ");
		ftoa(temp,s);
		lcdptr = 0x82;
		lcdws(s);	
	}
	return temp;
}
static void chksens(void)
{
	if (SENSOR_PIN & _BV(DOOR_PIN)) {
		beep(1,150);
	} 
	if (ALSENS_IPIN & _BV(ALSENS_PIN) == 0) {
		beep(1,150);
	}
}
static void displog(void)
{
	int8u i, status[7];
	char s[8];
	float dist;
	int16u co2;

	for (i = 0; i < 7; ++i) {
		status[i] = EEPROM_Read(INFO_EE_ADDR+i);
		dlyms(10);
	}

	lcdclr();
	lcdws ("LOG Parameters:");
	for (i = 0; i < 2; ++i) 
		if (status[i] == 1) {
			lcdclrr(1);
			lcdws(stat[i]);
			dlyms(2000);
			lcdclrr(1);
		}
	
	if (status[2] == 1) {
		lcdclrr(1);
		lcdws(stat[2]);
		dlyms(2000);
	} else if (status[2] == 2) {
		lcdclrr(1);
		lcdws(stat[3]);
		dlyms(2000);
	}
	
	lcdclrr(1);
	lcdws("Temp :   c");
	lcdwint(0xc6,status[3]);
	dlyms(2000);

	lcdclrr(1);
	lcdws("Dist :    mts");
	dist = status[4] / 100.0;
	ftoa(dist,s);
	lcdptr = 0xc6;
	lcdws(s);
	dlyms(2000);

	lcdclrr(1);
	lcdws("CO2 :    ppm");
	co2 = (status[5] * 256) + status[6];
	itoa(co2,s);
	lcdptr = 0xc6;
	lcdws(s);
	dlyms(2000);
	
	lcdclr();
	lcdws("  Please Reset");
	beep(2,500);

	sleep_enable();
	sleep_cpu();

}
static void logparam(void)
{
	int8u i, status[7];
	float dist;
	int16u co2;

	for (i = 0; i < 7; ++i)
		status[i] = 0;
	
	if (SENSOR_PIN & _BV(DOOR_PIN))
		status[0] = 1;

	if (ALSENS_IPIN & _BV(ALSENS_PIN) == 0)
		status[1] = 1;

	status[2] = collisens;

	status[3] = (int8u)meastemp(0);

	dist = measdist(0) * 100;
	status[4] = (int8u) (dist);

	co2 = measco2(0);
	status[5] = (co2 / 256);
	status[6] = (int8u)(co2 % 256);

	for (i = 0; i < 7; ++i) {
		EEPROM_Write(INFO_EE_ADDR+i, status[i]);
		dlyms(50);
	}
}
static int16u measco2(int8u disp)
{
	int16u co2;
	char s[8];

	co2 = 400 + (50 - (adcget(1) * STEP_SIZE * 100));

	if (disp == 1) {
		itoa(co2,s);
		lcdptr = 0x8a;
		lcdws("   ");
		lcdptr = 0x8a;
		lcdws(s);
	}
	return co2;
}
static void dispparam(void)
{
	lcdclr();
	lcdws("T:      C:   ppm");
	lcdptr = 0x87;
	lcdwc(lcdptr);
	lcdwd(0xdf);
	lcdr2();
	lcdws("VehiDist:    mts");
}

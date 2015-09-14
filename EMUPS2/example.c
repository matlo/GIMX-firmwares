#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include "usb_serial.h"
#include "avr/interrupt.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define LED_ON		(PORTD |= (1<<6))
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))


#define ATT_PIN		((PINB & 1)== 1)
#define CLK_PIN		((PINB & 2) > 0)

#define ACK_CONFIG	(DDRB |= (1<<7))
#define ACK_HIGH	(PORTB |= (1<<7))
#define ACK_LOW		(PORTB &= ~(1<<7))

#define TEST_CONFIG		(DDRD |= (1<<7))
#define TEST_HIGH		(PORTD |= (1<<7))
#define TEST_LOW		(PORTD &= ~(1<<7))

#define DDR_SPI DDRB
#define DD_MISO 3

#define TIME_OUT_ERROR_CODE 0xAA

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	
	/* Enable SPI */
	SPCR = 0;
	SPCR |= (1<<SPE);
//	SPCR |= (1<<SPIE);//interrupts
	SPCR |= (1<<DORD);
	SPCR |= (1<<CPOL);
	SPCR |= (1<<CPHA);
	
}

char SPI_SlaveReceive(char outData, char doACK)
{
    char inData = 0;//clear the status bit if it is pending
	char timeout = 0;
	
	ACK_HIGH;

	if(SPSR > 0)
	{
		inData = SPDR;
	}
	
	SPDR = outData;
	TEST_LOW;
	
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	{
		_delay_us(1);
		timeout++;
		if(timeout > 200)
		{
			return TIME_OUT_ERROR_CODE;
		}
	}
	
	TEST_HIGH;

	if(doACK)
	{
		ACK_LOW;
		//ack gets pulled back high at the beginning of next call
	}
	
	/* Return Data Register */
	inData = SPDR;
	return inData;
}

int counter = 0;

char buttons1 = 0xff;
char buttons2 = 0xff;

//used to buffer button releases until after report has been sent to console
char buttons1postReportState = 0xff;
char buttons2postReportState = 0xff;

char joyLX = 0x80;
char joyLY = 0x80;
char joyRX = 0x80;
char joyRY = 0x80;

char rightPressure = 0x00;
char leftPressure = 0x00;
char upPressure = 0x00;
char downPressure = 0x00;
char trianglePressure = 0x00;
char squarePressure = 0x00;
char crossPressure = 0x00;
char circlePressure = 0x00;
char L1Pressure = 0x00;
char R1Pressure = 0x00;
char L2Pressure = 0x00;
char R2Pressure = 0x00;

char configMode = 0;
char mode = 0x41;

char motorSetting1 = 0xff;
char motorSetting2 = 0xff;


char analogEnabled = 0;

char parseIndex = 0;
char recievedUpdate = 0;
int timeoutCounter = 0;


void printHex(char c)
{
	char n = (c >>4);
	if(n < 10)
		n += 48;
	else
		n += 55;
	usb_serial_putchar(n);
	n = (c & 0xf);
	if(n < 10)
		n += 48;
	else
		n += 55;
	usb_serial_putchar(n);
}

int main(void)
{	
	char cmd;
	char param1;
	char param2;
	char param3;
	
	cmd = 0;
	param1 = 0;
	param2 = 0;
	param3 = 0;

	DDRB = 0x00;
	PORTB |= 0x01;
	
	// set for 16 MHz clock, and make sure the LED is off
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_ON;

	SPI_SlaveInit();	
	ACK_CONFIG;
	ACK_HIGH;
	
	TEST_CONFIG;
		
	usb_init();
	_delay_ms(1000);

	usb_serial_flush_input();
	int flashCounter = 0;
	char LEDstate = 1;

	while (1) {

idle:
		TEST_HIGH;
		ACK_HIGH;
		SPDR = 0xff;
		if(ATT_PIN)
		{
			timeoutCounter++;
			if(timeoutCounter >= 30000)//we've been disconnected
			{
				mode = 0x41;
				motorSetting1 = 0xff;
				motorSetting2 = 0xff;
				analogEnabled = 0;
				configMode = 0;
				timeoutCounter = 0;
				flashCounter++;
				if(flashCounter > 5)
				{
					if(LEDstate == 1)
					{
						LED_OFF;
						LEDstate = 0;
					}
					else
					{
						LED_ON;
						LEDstate = 1;	
					}
					flashCounter = 0;
					
				}
			}
			if(usb_serial_available())
			{
				LED_ON;
				int c = usb_serial_getchar();
				if (c >= 0) 
				{
					switch(parseIndex)
					{
					case 0:
						if(c == 0x5A)
							parseIndex++;
						else
							usb_serial_putchar('x');
						break;
					case 1:
						buttons1 &= c;//combine this with any pending presses
						buttons1postReportState = c;
						parseIndex++;
						break;
					case 2:
						buttons2 &= c;//combine this with any pending presses
						buttons2postReportState = c;
						parseIndex++;
						break;
					case 3:
						joyRX = c;
						parseIndex++;
						break;
					case 4:
						joyRY = c;
						parseIndex++;
						break;
					case 5:
						joyLX = c;
						parseIndex++;
						break;
					case 6:
						joyLY = c;
						parseIndex = 0;
						recievedUpdate = 1;
						break;
					}
				}
				else
				{
					parseIndex = 0;
					usb_serial_putchar('x');
				}
			}
			_delay_us(1);
			goto idle;
		}
		timeoutCounter = 0;
		
		if((buttons1 & 0x10) == 0) upPressure = 0xFF; else upPressure = 0x00;
		if((buttons1 & 0x20) == 0) rightPressure = 0xFF; else rightPressure = 0x00;
		if((buttons1 & 0x40) == 0) downPressure = 0xFF; else downPressure = 0x00;
		if((buttons1 & 0x80) == 0) leftPressure = 0xFF; else leftPressure = 0x00;
		if((buttons2 & 0x01) == 0) L2Pressure = 0xFF; else L2Pressure = 0x00;
		if((buttons2 & 0x02) == 0) R2Pressure = 0xFF; else R2Pressure = 0x00;
		if((buttons2 & 0x04) == 0) L1Pressure = 0xFF; else L1Pressure = 0x00;
		if((buttons2 & 0x08) == 0) R1Pressure = 0xFF; else R1Pressure = 0x00;
		if((buttons2 & 0x10) == 0) trianglePressure = 0xFF; else trianglePressure = 0x00;
		if((buttons2 & 0x20) == 0) circlePressure = 0xFF; else circlePressure = 0x00;
		if((buttons2 & 0x40) == 0) crossPressure = 0xFF; else crossPressure = 0x00;
		if((buttons2 & 0x80) == 0) squarePressure = 0xFF; else squarePressure = 0x00;

		cli();//disable usb interrupts
		LED_ON;
		TEST_LOW;
		cmd = SPI_SlaveReceive(0xff,1);		
		TEST_HIGH;
		
		if(cmd != 0x01)
			goto finish;
		
		if(configMode)
		{
			cmd = SPI_SlaveReceive(0xF3,1);		
			SPI_SlaveReceive(0x5A,1);
			switch(cmd)
			{
				case 0x40:
					param1 = SPI_SlaveReceive(0x00,1);
//					if(param1 == 0) //lots on param1 but the reponses are all the same?
					{
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x02,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x5A,0);
					}
					break;
				case 0x41://check response data
					if(analogEnabled==1)
					{
						//button pressures byte mask, appears after 0x44 is called
						SPI_SlaveReceive(0xFF,1);//use mask instead?
						SPI_SlaveReceive(0xFF,1);
						SPI_SlaveReceive(0x03,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x5A,0);
					}
					else
					{
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,0);
					}
					break;
				case 0x43://exit config mode
					param1 = SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,0);
					if(param1 == 0)
					{
						configMode = 0;
					}
					break;
				case 0x44://turn on analog mode
					param1 = SPI_SlaveReceive(0x00,1);
					param2 = SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,0);
					if(param1 == 1)
					{
						mode = 0x73;//no analog button pressures
						analogEnabled = 1;
					}
					else
					{
						mode = 0x41;	
						analogEnabled = 0;
					}
					break;
				case 0x45://query model
					param1 = SPI_SlaveReceive(0x03,1);
					param2 = SPI_SlaveReceive(0x02,1);
					if(analogEnabled==1)
						SPI_SlaveReceive(0x01,1);
					else
						SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x02,1);
					SPI_SlaveReceive(0x01,1);
					SPI_SlaveReceive(0x00,0);
					break;
				case 0x46:
					param1 = SPI_SlaveReceive(0x00,1);
					if(param1 == 0)
					{
						param2 = SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x01,1);
						SPI_SlaveReceive(0x02,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x0F,0);
					}
					if(param1 == 1)
					{
						param2 = SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x01,1);
						SPI_SlaveReceive(0x01,1);
						SPI_SlaveReceive(0x01,1);
						SPI_SlaveReceive(0x0F,0);
					}
					break;
				case 0x47:
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x02,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x01,1);
					SPI_SlaveReceive(0x00,0);
					break;
				case 0x4C:
					param1 = SPI_SlaveReceive(0x00,1);
					if(param1 == 0)
					{
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x04,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,0);
					}
					if(param1 == 1)
					{
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x07,1);
						SPI_SlaveReceive(0x00,1);
						SPI_SlaveReceive(0x00,0);
					}
					break;
				case 0x4D://setup motors
					motorSetting1 = SPI_SlaveReceive(motorSetting1,1);
					motorSetting2 = SPI_SlaveReceive(motorSetting2,1);
					SPI_SlaveReceive(0xFF,1);
					SPI_SlaveReceive(0xFF,1);
					SPI_SlaveReceive(0xFF,1);
					SPI_SlaveReceive(0xFF,0);
					break;
				case 0x4F://set pressure byte mask, extended pressures
					mode = 0x79;//analog button pressures
					param1 = SPI_SlaveReceive(0x00,1);
					param2 = SPI_SlaveReceive(0x00,1);
					param3 = SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x00,1);
					SPI_SlaveReceive(0x5A,0);
					break;
			}
		}
		else
		{
			TEST_LOW;
			cmd = SPI_SlaveReceive(mode,1);	
			
			SPI_SlaveReceive(0x5A,1);
			
			if(mode == 0x41)//digital only
			{
				param1 = SPI_SlaveReceive(buttons1,1);
				param2 = SPI_SlaveReceive(buttons2,0);
			}
			if(mode == 0x73)//digital buttons, analog joysticks
			{
				param1 = SPI_SlaveReceive(buttons1,1);
				if(param1 ==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(buttons2,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyRX,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyRY,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyLX,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyLY,0)==TIME_OUT_ERROR_CODE) goto finish;
			}
			if(mode == 0x79)//analog joysticks, analog buttons
			{
				param1 = SPI_SlaveReceive(buttons1,1);
				if(param1 ==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(buttons2,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyRX,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyRY,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyLX,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(joyLY,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(rightPressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(leftPressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(upPressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(downPressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(trianglePressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(circlePressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(crossPressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(squarePressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(L1Pressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(R1Pressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(L2Pressure,1)==TIME_OUT_ERROR_CODE) goto finish;
				if(SPI_SlaveReceive(R2Pressure,0)==TIME_OUT_ERROR_CODE) goto finish;
			}
			//cmd should usually be 0x42 for polling
			if(cmd == 0x43)
			{
				if(param1 == 0x01)
					configMode = 1;
			}
		}
finish:
		TEST_HIGH;
		ACK_HIGH;
		if((buttons1 == 0xff)&&(buttons2 == 0xff))
		{
			LED_OFF;
		}

		//copy queued releases into button state
		buttons1 = buttons1postReportState;
		buttons2 = buttons2postReportState;
		sei();//renable usb interrupts
		
		if(recievedUpdate == 1)
		{
			//ack the update was sent to the console
			usb_serial_putchar('k');
			recievedUpdate = 0;
		}

		while(!ATT_PIN)
		{
			_delay_us(20); //conservative so we aren't still in ATT low at the top of the loop		
		}

		
		counter++;
	}
}
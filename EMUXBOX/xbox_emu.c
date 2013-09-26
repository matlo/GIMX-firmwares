/*
  Copyright 2013  Mathieu Laurendeau (mat.lau [at] laposte [dot] net)

  Redistributed under the GPLv3 licence.

  Based on code by
    Denver Gingerich (denver [at] ossguy [dot] com)
    Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Original licence:

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the Xbox v1 controller. This file contains the main tasks and
 *  is responsible for the initial application hardware configuration.
 */
 
#include "xbox_emu.h"
#include <LUFA/Drivers/Peripheral/Serial.h>

/*
 * The report data.
 * see http://euc.jp/periphs/xbox-controller.ja.html
 */
static uint8_t report[20] = {
    0x00,
    0x14,//size
    0x00,//digital buttons:
         //d-pad up
         //d-pad down
         //d-pad left
         //d-pad right
         //start
         //back
         //left stick
         //right stick
    0x00,//unused
    0x00,//A
    0x00,//B
    0x00,//X
    0x00,//Y
    0x00,//black
    0x00,//white
    0x00,//left trigger
    0x00,//right trigger
    0x00,0x00,//left stick x
    0x00,0x00,//left stick y
    0x00,0x00,//right stick x
    0x00,0x00,//right stick y
};

#define USART_BAUDRATE 500000
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

static uint8_t* pdata = report;
static unsigned char i = 0;

static unsigned char led = 0;
static unsigned char j = 0;

#define LED_ON (PORTD |= (1<<6))
#define LED_OFF (PORTD &= ~(1<<6))

static unsigned char sendReport = 0;

void Serial_Task(void)
{
  if((UCSR1A & (1 << RXC1)))
  {
    while(i < sizeof(report))
    {
      pdata[i++] = Serial_RxByte();
    }
    i = 0;
    sendReport = 1;
    ++j;
    if(!j)
    {
      if(led)
      {
        LED_OFF;
        led = 0;
      }
      else
      {
        LED_ON;
        led = 1;
      }
    }
  }
}

/*ISR(USART1_RX_vect)
{
  pdata[i] = UDR1; // Fetch the received byte value
  //UDR1 = pdata[i]; // Echo back the received byte back to the computer

  i++;
  i%=sizeof(report);
}*/

void serial_init(void)
{
   UBRR1H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
   UBRR1L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

   UCSR1B |= (1 << RXEN1) | (1 << TXEN1); // Turn on the transmission and reception circuitry

   UCSR1C |= _BV(UCSZ10) | _BV(UCSZ11); /* 8 bits per char */

   //UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)

   return;
}

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

	SetupHardware();

	for (;;)
	{
	  Serial_Task();
		HID_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

  serial_init();

	/* Hardware Initialization */
	//LEDs_Init();
  DDRD |= (1<<6);
  USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{	
	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDMASK_USB_READY);

  /* Setup IN Report Endpoint */
  if (!(Endpoint_ConfigureEndpoint(XBOX_IN_EPNUM, EP_TYPE_INTERRUPT,
                                 ENDPOINT_DIR_IN, XBOX_EPSIZE,
                                   ENDPOINT_BANK_SINGLE)))
  {
    LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
  }

	/* Setup OUT Report Endpoint */
	/*if (!(Endpoint_ConfigureEndpoint(XBOX_OUT_EPNUM, EP_TYPE_INTERRUPT,
		                             ENDPOINT_DIR_OUT, XBOX_EPSIZE,
	                                 ENDPOINT_BANK_SINGLE)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}*/
	
	//USB_Device_EnableSOFEvents();
}

/** Event handler for the USB_UnhandledControlRequest event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the HID commands, which are
 *  all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
void EVENT_USB_Device_UnhandledControlRequest(void)
{

}

/** Event handler for the USB device Start Of Frame event. */
/*void EVENT_USB_Device_StartOfFrame(void)
{
	// One millisecond has elapsed.
}*/

/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
	/* Select the IN Report Endpoint */
	Endpoint_SelectEndpoint(XBOX_IN_EPNUM);

  //Endpoint_SetEndpointDirection(ENDPOINT_DIR_IN);

	if (sendReport)
	{
	  /* Wait until the host is ready to accept another packet */
	  while (!Endpoint_IsINReady()) {}

		/* Write IN Report Data */
		Endpoint_Write_Stream_LE(report, sizeof(report));
		
		sendReport = 0;

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
	}

  //Endpoint_SetEndpointDirection(ENDPOINT_DIR_OUT);
}

/** Reads the next OUT report from the host from the OUT endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
	/* Select the OUT Report Endpoint */
	Endpoint_SelectEndpoint(XBOX_OUT_EPNUM);

	/* Check if OUT Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Discard data */
			Endpoint_Discard_Stream(6);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}

/** Function to manage HID report generation and transmission to the host, when in report mode. */
void HID_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;
	  
	/* Send the next keypress report to the host */
	SendNextReport();
		
	/* Process the LED report sent from the host */
	ReceiveNextReport();
}

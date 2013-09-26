/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *  Main source file for the Joystick demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */

#include "Joystick.h"
#include <LUFA/Drivers/Peripheral/Serial.h>

#define USART_BAUDRATE 500000

#define LED_PIN 6

#define LED_ON (PORTD |= (1<<LED_PIN))
#define LED_OFF (PORTD &= ~(1<<LED_PIN))

/*
 * The reference report data.
 */
static uint8_t report[12] =
{
    0x00,0x00,//xaxis
    0x00,0x00,//yaxis
    0x00,0x00,//zaxis
    0x00,0x00,//rzaxis
    0x00,0x00,//hat
    0x00,0x00,//buttons
};

static uint8_t* pdata = report;
static unsigned char i = 0;

static unsigned char sendReport = 0;

static inline int16_t Serial_BlockingReceiveByte(void)
{
  while(!Serial_IsCharReceived());
  return UDR1;
}

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();

  GlobalInterruptEnable();

	for (;;)
	{
		HID_Task();
		USB_USBTask();
	}
}

ISR(USART1_RX_vect)
{
  pdata[i++] = UDR1;
  while(i < sizeof(report))
  {
    pdata[i++] = Serial_BlockingReceiveByte();
  }
  i = 0;
  sendReport = 1;
}

void serial_init(void)
{
  Serial_Init(USART_BAUDRATE, false);

  UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)
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
  DDRD |= (1<<LED_PIN);

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
 *  the status LEDs and stops the USB management and joystick reporting tasks.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the joystick reporting task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoint */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_EPNUM, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/*
 * Series of bytes that appear in control packets right after the HID
 * descriptor is sent to the host. They where discovered by tracing output
 * from a Madcatz SF4 Joystick. Sending these bytes makes the PS button work.
 */
static const uint8_t PROGMEM magic_init_bytes[] =
{
  0x21, 0x26, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00
};

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  /* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
			  Endpoint_ClearSETUP();
			  if(USB_ControlRequest.wValue == 0x0300)
			  {
			    /* Write the report data to the control endpoint */
          Endpoint_Write_Control_PStream_LE(magic_init_bytes, sizeof(magic_init_bytes));
			  }
			  else
			  {
			    /* Write the report data to the control endpoint */
          Endpoint_Write_Control_Stream_LE(report, sizeof(report));
			  }

				Endpoint_ClearOUT();
			}

			break;
	}
}

/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
  /* Select the IN Report Endpoint */
  Endpoint_SelectEndpoint(JOYSTICK_EPNUM);

  if (sendReport)
  {
    /* Wait until the host is ready to accept another packet */
    while (!Endpoint_IsINReady()) {}

    /* Write IN Report Data */
    Endpoint_Write_Stream_LE(report, sizeof(report), NULL);

    sendReport = 0;

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
  }
}

/** Function to manage HID report generation and transmission to the host. */
void HID_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

  /* Send the next keypress report to the host */
  SendNextReport();
}

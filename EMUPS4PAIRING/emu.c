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
 *  Main source file for the controller. This file contains the main tasks and
 *  is responsible for the initial application hardware configuration.
 */
 
#include "emu.h"

uint8_t EEMEM eeSlaveBdaddr[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
uint8_t EEMEM eeMasterBdaddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t EEMEM eeLinkKey[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint8_t slaveBdaddr[6];
uint8_t masterBdaddr[6];
uint8_t linkKey[16];

/*
 * Read data from the eeprom.
 */
void readEepromData(void)
{
  eeprom_read_block((void*)&slaveBdaddr, (const void*)&eeSlaveBdaddr, sizeof(slaveBdaddr));
  eeprom_read_block((void*)&masterBdaddr, (const void*)&eeMasterBdaddr, sizeof(masterBdaddr));
  eeprom_read_block((void*)&linkKey, (const void*)&eeLinkKey, sizeof(linkKey));
}

/*
 * The reference report data.
 */
static uint8_t report[] = {
    0x01, //Report ID
    0x80, 0x80, 0x80, 0x80, //X, Y, Z, Rz
    0x08, //4 MSB = 4 buttons, 4 LSB = hat switch
    0x00, 0x00, //10 MSB = 10 buttons, 6 LSB = vendor defined
    0x00, 0x00, //Rx, Ry
    //The rest of the report is unknown.
    0x93, 0x5F, 0xFB, 0xD2, 0xFF, 0xDA,
    0xFF, 0xD8, 0xFF, 0x4F, 0xEE, 0x14, 0x1B, 0x99,
    0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00
};

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
  readEepromData();

  SetupHardware();

  for (;;)
  {
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

  GlobalInterruptEnable();

	/* Hardware Initialization */
	LEDs_Init();

  USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{	
	//LEDs_SetAllLEDs(LEDMASK_USB_READY);

	if (!(Endpoint_ConfigureEndpoint(IN_EPNUM, EP_TYPE_INTERRUPT, EPSIZE, 1)))
	{
		//LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	if (!(Endpoint_ConfigureEndpoint(OUT_EPNUM, EP_TYPE_INTERRUPT, EPSIZE, 1)))
	{
		//LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	//USB_Device_EnableSOFEvents();
}

const char PROGMEM bufa3[] = {
  0xA3, 0x41, 0x75, 0x67, 0x20, 0x20, 0x33, 0x20,
  0x32, 0x30, 0x31, 0x33, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x30, 0x37, 0x3A, 0x30, 0x31, 0x3A, 0x31,
  0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x31, 0x03, 0x00, 0x00,
  0x00, 0x49, 0x00, 0x05, 0x00, 0x00, 0x80, 0x03,
  0x00
};

const char PROGMEM buf02[] = {
  0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87,
  0x22, 0x7B, 0xDD, 0xB2, 0x22, 0x47, 0xDD, 0xBD,
  0x22, 0x43, 0xDD, 0x1C, 0x02, 0x1C, 0x02, 0x7F,
  0x1E, 0x2E, 0xDF, 0x60, 0x1F, 0x4C, 0xE0, 0x3A,
  0x1D, 0xC6, 0xDE, 0x08, 0x00
};

const char PROGMEM buf12[] = {
  0x12,
  0x66, 0x55, 0x44, 0x33, 0x22, 0x11,//slave bdaddr
  0x08, 0x25,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//master bdaddr
  0x00
};

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  static char buffer[FIXED_CONTROL_ENDPOINT_SIZE];
  unsigned char len;

  /* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
        if(USB_ControlRequest.wValue == 0x03a3)
        {
          memcpy_P(buffer, bufa3, sizeof(bufa3));
          len = sizeof(bufa3);
        }
        else if(USB_ControlRequest.wValue == 0x0302)
        {
          memcpy_P(buffer, buf02, sizeof(buf02));
          len = sizeof(buf02);
        }
        else if(USB_ControlRequest.wValue == 0x0312)
        {
          memcpy_P(buffer, buf12, sizeof(buf12));
          len = sizeof(buf12);
          memcpy(buffer+1, slaveBdaddr, sizeof(slaveBdaddr));
          memcpy(buffer+10, masterBdaddr, sizeof(masterBdaddr));
        }
        else if(USB_ControlRequest.wValue == 0x0313)
        {
          /*
           * Not in the original DS4.
           * Added for getting the link key.
           */
          memcpy(buffer, linkKey, sizeof(linkKey));
          len = sizeof(linkKey);
        }
        else
        {
          break;
        }
        Endpoint_ClearSETUP();
        Endpoint_Write_Control_Stream_LE(buffer, len);
        Endpoint_ClearOUT();
			}
		
			break;
		case REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
				Endpoint_ClearIN();

				if(USB_ControlRequest.wValue == 0x0312)
				{
          /*
           * Not in the original DS4.
           * Added for setting the slave bdaddr.
           */
					memcpy(slaveBdaddr, buffer, sizeof(slaveBdaddr));
					eeprom_write_block(slaveBdaddr, &eeSlaveBdaddr, sizeof(slaveBdaddr));
				}
				else if(USB_ControlRequest.wValue == 0x0313)
        {
          memcpy(masterBdaddr, buffer+1, sizeof(masterBdaddr));
          memcpy(linkKey, buffer+7, sizeof(linkKey));
          eeprom_write_block(masterBdaddr, &eeMasterBdaddr, sizeof(masterBdaddr));
          eeprom_write_block(linkKey, &eeLinkKey, sizeof(linkKey));
        }
				else if(USB_ControlRequest.wValue == 0x0314)
        {
          //anything to do there?
        }
			}
			
			break;

	}
}

/** Event handler for the USB device Start Of Frame event. */
/*void EVENT_USB_Device_StartOfFrame(void)
{
	// One millisecond has elapsed.
}*/

static volatile unsigned char sendReport = 0;

/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
	/* Select the IN Report Endpoint */
	Endpoint_SelectEndpoint(IN_EPNUM);

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

/** Reads the next OUT report from the host from the OUT endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
	/* Select the OUT Report Endpoint */
	Endpoint_SelectEndpoint(OUT_EPNUM);

	/* Check if OUT Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Discard data */
			Endpoint_Discard_8();
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

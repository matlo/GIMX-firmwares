/*
  Copyright 2019  Mathieu Laurendeau (mat.lau [at] laposte [dot] net)

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
 
#include "xbox_emu.h"
#include <LUFA/Drivers/Peripheral/Serial.h>
#include "../adapter_protocol.h"

#define MAX_CONTROL_TRANSFER_SIZE 64

#define USART_BAUDRATE 500000
#define USART_DOUBLE_SPEED false

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

static uint8_t* pdata;
static unsigned char i = 0;

/*
 * These variables are used in both the main and the serial interrupt,
 * therefore they have to be declared as volatile.
 */
static volatile unsigned char sendReport = 1; // first input report is above
static volatile unsigned char started = 0;
static volatile unsigned char packet_type = 0;
static volatile unsigned char value_len = 0;
static volatile unsigned char initialized = 1;

void forceHardReset(void)
{
  LEDs_TurnOnLEDs(LEDS_ALL_LEDS);
  cli(); // disable interrupts
  wdt_enable(WDTO_15MS); // enable watchdog
  while(1); // wait for watchdog to reset processor
}

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

  for (;;)
  {
    HID_Task();
    USB_USBTask();
  }
}

static inline void handle_packet(void)
{
  switch(packet_type)
  {
    case BYTE_TYPE:
      Serial_SendByte(BYTE_TYPE);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(BYTE_TYPE_XBOX);
      break;
    case BYTE_STATUS:
      Serial_SendByte(BYTE_STATUS);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(started);
      break;
    case BYTE_START:
      Serial_SendByte(BYTE_START);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(started);
      started = 1;
      break;
    case BYTE_RESET:
      forceHardReset();
      break;
    case BYTE_IN_REPORT:
      sendReport |= initialized;
      //no answer
      break;
  }
}

static unsigned char buf[MAX_CONTROL_TRANSFER_SIZE];

ISR(USART1_RX_vect)
{
  packet_type = UDR1;
  value_len = Serial_BlockingReceiveByte();
  if(packet_type == BYTE_IN_REPORT && initialized)
  {
    pdata = report;
  }
  else
  {
    pdata = buf;
  }
  while(i < value_len)
  {
    pdata[i++] = Serial_BlockingReceiveByte();
  }
  i = 0;
  handle_packet();
}

void serial_init(void)
{
  Serial_Init(USART_BAUDRATE, USART_DOUBLE_SPEED);

  UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
  /* Disable watchdog */
  MCUSR = 0;
  wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

  serial_init();

  GlobalInterruptEnable();

	/* Hardware Initialization */
	LEDs_Init();

  while(!started);

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

  if (!(Endpoint_ConfigureEndpoint(XBOX_IN_EPNUM, EP_TYPE_INTERRUPT, XBOX_EPSIZE, 1)))
  {
    //LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
  }

  if (!(Endpoint_ConfigureEndpoint(XBOX_OUT_EPNUM, EP_TYPE_INTERRUPT, XBOX_EPSIZE, 1)))
  {
    //LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
  }
	
	//USB_Device_EnableSOFEvents();
}

const char PROGMEM vendor1[] =
{
        0x10, 0x42, 0x00, 0x01, 0x01, 0x02, 0x14, 0x06,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const char PROGMEM vendor2[] =
{
        0x00, 0x06, 0xff, 0xff, 0xff, 0xff
};

const char PROGMEM vendor3[] =
{
        0x00, 0x14, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
};

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
    if(USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_INTERFACE))
    {
        if (USB_ControlRequest.bRequest == 6) // vendor-defined request
        {
            if (USB_ControlRequest.wValue == 0x4200)
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor1, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
        }
        else if (USB_ControlRequest.bRequest == REQ_GetReport)
        {
            if (USB_ControlRequest.wValue == 0x0200)
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor2, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
            else if (USB_ControlRequest.wValue == 0x0100)
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor3, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
        }
    }
    else if(USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
    {
        if (USB_ControlRequest.bRequest == REQ_GetReport)
        {
            if (USB_ControlRequest.wValue == 0x0100)
            {
                report[12] = report[14] = report[16] = report[18] = 0x80;
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(report, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
                initialized = 1;
            }
        }
    }

    if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST
            && USB_ControlRequest.bRequest == REQ_GetDescriptor
            && (USB_ControlRequest.wValue >> 8) == DTYPE_Configuration
            && USB_ControlRequest.wLength == 0x50) {
        /*
         * Assume Xbox console is connected.
         * Report a first input report, then inhibit input reports until it is requested on control endpoint.
         */
        initialized = 0;
    }
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
		Endpoint_Write_Stream_LE(report, sizeof(report), NULL);
		
		sendReport = 0;

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
	}

  //Endpoint_SetEndpointDirection(ENDPOINT_DIR_OUT);
}

/** Reads the next OUT report from the host from the OUT endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
    static struct
    {
        struct
        {
            unsigned char type;
            unsigned char length;
        } header;
        unsigned char buffer[XBOX_EPSIZE];
    } packet = { .header.type = BYTE_OUT_REPORT };

    uint16_t length = 0;

    /* Select the OUT Report Endpoint */
    Endpoint_SelectEndpoint(XBOX_OUT_EPNUM);

    /* Check if OUT Endpoint contains a packet */
    if (Endpoint_IsOUTReceived())
    {

        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Read OUT Report Data */
            uint8_t ErrorCode = Endpoint_Read_Stream_LE(packet.buffer, sizeof(packet.buffer), &length);
            if (ErrorCode == ENDPOINT_RWSTREAM_NoError)
            {
                length = sizeof(packet.buffer);
            }
        }

        /* Handshake the OUT Endpoint - clear endpoint and ready for next report */
        Endpoint_ClearOUT();

        if (length)
        {
            packet.header.length = length & 0xFF;
            Serial_SendData(&packet, sizeof(packet.header) + packet.header.length);
        }
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

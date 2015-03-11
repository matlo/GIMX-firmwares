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
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <avr/wdt.h>
#include "../adapter_protocol.h"

#define USART_BAUDRATE 500000
#define USART_DOUBLE_SPEED false

/*
 * The reference report data.
 */
static uint8_t report[20] = {
      0x00,//type
      0x14,//size
      0x00,0x00,//buttons
      0x00,//ltrigger
      0x00,//rtrigger
      0x00,0x00,//xaxis
      0x00,0x00,//yaxis
      0x00,0x00,//zaxis
      0x00,0x00,//taxis
      0x00,0x00,0x00,0x00,0x00,0x00
};

static unsigned char ready = 0;

static uint8_t* pdata;
static unsigned char i = 0;

/*
 * These variables are used in both the main and serial interrupt,
 * therefore they have to be declared as volatile.
 */
static volatile unsigned char sendReport = 0;
static volatile unsigned char started = 0;
static volatile unsigned char packet_type = 0;
static volatile unsigned char value_len = 0;
static volatile unsigned char spoofReply = 0;
static volatile unsigned char spoofReplyLen = 0;
static volatile unsigned char spoof_initialized = BYTE_STATUS_NSPOOFED;

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

static inline void send_spoof_header(void)
{
  Serial_SendByte(BYTE_SPOOF_DATA);
  if( USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST )
  {
    Serial_SendByte(sizeof(USB_ControlRequest));
  }
  else
  {
    Serial_SendByte(sizeof(USB_ControlRequest) + (USB_ControlRequest.wLength & 0xFF));
  }
  Serial_SendData(&USB_ControlRequest, sizeof(USB_ControlRequest));
}

static inline void handle_packet(void)
{
  switch(packet_type)
  {
    case BYTE_TYPE:
      Serial_SendByte(BYTE_TYPE);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(BYTE_TYPE_X360);
      break;
    case BYTE_STATUS:
      Serial_SendByte(BYTE_STATUS);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(spoof_initialized);
      break;
    case BYTE_START:
      Serial_SendByte(BYTE_START);
      Serial_SendByte(BYTE_LEN_1_BYTE);
      Serial_SendByte(spoof_initialized);
      started = 1;
      break;
    case BYTE_SPOOF_DATA:
      spoofReply = 1;
      spoofReplyLen = value_len;
      break;
    case BYTE_RESET:
      forceHardReset();
      break;
    case BYTE_SEND_REPORT:
      sendReport = 1;
      //no answer
      break;
  }
}

static unsigned char buf[64];

ISR(USART1_RX_vect)
{
  packet_type = UDR1;
  value_len = Serial_BlockingReceiveByte();
  if(packet_type == BYTE_SEND_REPORT)
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

static unsigned char response = 0;

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  static char buffer[FIXED_CONTROL_ENDPOINT_SIZE];

  if(USB_ControlRequest.bmRequestType & REQTYPE_VENDOR)
  {
    if(!spoof_initialized)
    {
      if( !(USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) )
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();
      }
      spoofReply = 0;
      send_spoof_header();
      if( USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST )
      {
        while(!spoofReply);
        Endpoint_ClearSETUP();
        Endpoint_Write_Control_Stream_LE(buf, spoofReplyLen);
        Endpoint_ClearOUT();
        if(USB_ControlRequest.wValue == 0x5c10)
        {
          if(response)
          {
            spoof_initialized = BYTE_STATUS_SPOOFED;
            LEDs_TurnOffLEDs(LEDS_LED1);
            ready = 1;
          }
          response = 1;
        }
        else if(USB_ControlRequest.wValue == 0x5b17)
        {
          LEDs_TurnOnLEDs(LEDS_LED1);
        }
      }
      else
      {
        Serial_SendData(buffer, USB_ControlRequest.wLength);
      }
    }
    else
    {
      if( USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST )
      {
        if(USB_ControlRequest.wValue == 0x5b17)
        {
          forceHardReset();
        }
        Endpoint_ClearSETUP();
        if(USB_ControlRequest.bRequest == 0x01)
        {
          if(USB_ControlRequest.bmRequestType & REQREC_INTERFACE)
          {
            char tmp[20] =
            {
                0x00,0x14,0xff,0xf7,0xff,0xff,0xc0,0xff,
                0xc0,0xff,0xc0,0xff,0xc0,0xff,0x00,0x00,
                0x00,0x00,0x00,0x00
            };
            Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
          }
          else
          {
            char tmp[4] = {0x01,0x00,0x83,0x66};
            Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
          }
        }
        else if(USB_ControlRequest.bRequest == 0xa1)
        {
          char tmp[2] = {0x01,0x02};
          Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
        }
        Endpoint_ClearOUT();
      }
      else
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();
      }
    }
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
	Endpoint_SelectEndpoint(IN_EPNUM);

  if (ready && sendReport)
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
  static struct
  {
    struct
    {
      unsigned char type;
      unsigned char length;
    } header;
    unsigned char buffer[EPSIZE];
  } packet = { .header.type = BYTE_OUT_REPORT };

  uint16_t length = 0;

  /* Select the OUT Report Endpoint */
	Endpoint_SelectEndpoint(OUT_EPNUM);

  /* Check if OUT Endpoint contains a packet */
  if (Endpoint_IsOUTReceived())
  {
    /* Check to see if the packet contains data */
    if (Endpoint_IsReadWriteAllowed())
    {
		  /* Read OUT Report Data */
		  uint8_t ErrorCode = Endpoint_Read_Stream_LE(packet.buffer, sizeof(packet.buffer), &length);
		  if(ErrorCode == ENDPOINT_RWSTREAM_NoError)
		  {
		    length = sizeof(packet.buffer);
		  }
    }

    /* Handshake the OUT Endpoint - clear endpoint and ready for next report */
    Endpoint_ClearOUT();

		if(length)
		{
		  switch(packet.buffer[0])
		  {
		    default:
		      packet.header.length = length & 0xFF;
		      Serial_SendData(&packet, sizeof(packet.header) + packet.header.length);
		      break;
		  }
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

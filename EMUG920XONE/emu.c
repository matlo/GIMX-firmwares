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
#include "../adapter_protocol.h"

#define LED_CONFIG  (DDRD |= (1<<6))
#define LED_OFF   (PORTD &= ~(1<<6))
#define LED_ON    (PORTD |= (1<<6))

#define MAX_CONTROL_TRANSFER_SIZE 64

#define USART_BAUDRATE 500000
#define USART_DOUBLE_SPEED false

#define REPORT_TYPE_INPUT 0x01
#define REPORT_TYPE_FEATURE 0x03

/*
 * The reference report data.
 */
static struct __attribute__ ((packed))
{
  unsigned char reportId; //0x01
  unsigned char hatAndButtons;
  unsigned char buttons1;
  unsigned char buttons2; //last 2 bits are unused
  unsigned short wheel;
  unsigned char gas;
  unsigned char brake;
  unsigned char clutch;
  unsigned char buttons3;
} report01 = { 0x01, 0x08, 0x00, 0x00, 0x8000, 0xff, 0xff, 0xff, 0x00 };

static unsigned char report11[20] = { 0x11 };
static unsigned char report12[64] = { 0x12 };

static unsigned char ready = 1;

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

void forceHardReset(void)
{
  LED_ON;
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
  Serial_SendByte(BYTE_CONTROL_DATA);
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
      Serial_SendByte(BYTE_TYPE_G920_XONE);
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
    case BYTE_CONTROL_DATA:
      spoofReply = 1;
      spoofReplyLen = value_len;
      break;
    case BYTE_RESET:
      forceHardReset();
      break;
    case BYTE_IN_REPORT:
      sendReport = 1;
      //no answer
      break;
  }
}

static unsigned char buf[MAX_CONTROL_TRANSFER_SIZE];

ISR(USART1_RX_vect)
{
  packet_type = UDR1;
  value_len = Serial_BlockingReceiveByte();
  if(packet_type == BYTE_IN_REPORT)
  {
    pdata = (uint8_t*)&report01;
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

  LED_CONFIG;

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

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  static unsigned char buffer[MAX_CONTROL_TRANSFER_SIZE];

  Serial_SendByte(BYTE_DEBUG);
  Serial_SendByte(sizeof(USB_ControlRequest));
  Serial_SendData(&USB_ControlRequest, sizeof(USB_ControlRequest));

  /* Handle HID Class specific requests */
  switch (USB_ControlRequest.bRequest)
  {
    case REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        const void * reportBuf = NULL;
        unsigned char len = 0;

        if(reportType == REPORT_TYPE_INPUT)
        {
          switch(reportId)
          {
          case 0x01:
            reportBuf = &report01;
            len = sizeof(report01);
            break;
          case 0x11:
            reportBuf = report11;
            len = sizeof(report11);
            break;
          case 0x12:
            reportBuf = report12;
            len = sizeof(report12);
            break;
          }
          if(len)
          {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(reportBuf, len);
            Endpoint_ClearOUT();
          }
        }
      }
      break;
    case REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();

        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        if(reportType == REPORT_TYPE_FEATURE)
        {
          switch(reportId)
          {
            default:
              Serial_SendByte(BYTE_DEBUG);
              Serial_SendByte(BYTE_LEN_1_BYTE);
              Serial_SendByte(reportId);
              break;
          }
        }
      }
      break;
    case REQ_SetIdle:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();
      }
      break;
  }
}

/** Event handler for the USB device Start Of Frame event. */
/*void EVENT_USB_Device_StartOfFrame(void)
{
  // One millisecond has elapsed.
}*/

#ifdef REPORT_NB_INFO
static unsigned char nbReports = 0;
static unsigned char info[] = { BYTE_DEBUG, BYTE_LEN_0_BYTE };
#endif

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
    Endpoint_Write_Stream_LE(&report01, sizeof(report01), NULL);

    sendReport = 0;

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();

#ifdef REPORT_NB_INFO
    nbReports++;
    if(!nbReports) {
      Serial_SendData(info, sizeof(info));
    }
#endif
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
  } packet = { .header.type = BYTE_DEBUG };

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

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

#define MAX_CONTROL_TRANSFER_SIZE 64

#define USART_BAUDRATE 500000
#define USART_DOUBLE_SPEED false

#define REPORT_TYPE_FEATURE 0x03

#define configPin = 11

/*
 * Config for the PSX model
 * PS4 = 0
 * PS3 = 1
 */

short int consoleModel = 0;


/*
 * The master bdaddr.
 */
static uint8_t masterBdaddr[6];

/*
 * A byte to save.
 */
static uint8_t byte_6_ef;

/*
 * Indicates if the master bdaddr was already requested or not.
 */
static unsigned char reply = 0;


/*
 * The reference report data.
 */
static uint8_t reportPS4[] = {
    0x01, //Report ID
    0x80, 0x80, 0x80, 0x80, //X, Y, Z, Rz
    0x08, //4 MSB = 4 buttons, 4 LSB = hat switch
    0x00, 0x00, //10 MSB = 10 buttons, 6 LSB = vendor defined
    0x00, 0x00, //Rx, Ry
    //The rest of the report is unknown.
    0x99, 0x50, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xe7, 0xff, 0x6e, 0x20, 0xd9,
    0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00
};

/*
 * The reference report data.
 */
static uint8_t reportPS3[49] = {
   0x01, 0x00, 0x00, 0x00,
   0x00, //0x01 = PS3 button
   0x00, 0x7a, 0x80, 0x82,
   0x7d, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00,
   0x02, 0xee, 0x10, 0x00,
   0x00, 0x00, 0x00, 0x02,
   0xff, 0x77, 0x01, 0x80,
   0x02, 0x1f, 0x02, 0x02,
   0x01, 0x9c, 0x00, 0x05
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
      Serial_SendByte(BYTE_TYPE_DS4);
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
      if (consoleModel == 0){
        spoofReply = 1;
        spoofReplyLen = value_len;
      }
      break;
    case BYTE_RESET:
      if (consoleModel == 0){
        forceHardReset();
      }
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
    if(consoleModel == 0)
    {
      pdata = reportPS4;
    }
    else
    {
      pdata = reportPS3;
    }
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
  if(consoleModel == 0){
    MCUSR = 0;
  } else {
    MCUSR &= ~(1 << WDRF);
  }

 /* Disable watchdog if enabled by bootloader/fuses */
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

const char PROGMEM buff3[] = {
  0xF3, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00
};

const char PROGMEM buf03[] =
{
  0x03, 0x21, 0x27, 0x04, 0x41, 0x00, 0x2c, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const char PROGMEM report_01[] = {
    //Sixaxis
    /*0x00, 0x01, 0x03, 0x00, 0x04, 0x0c, 0x01, 0x02,
    0x18, 0x18, 0x18, 0x18, 0x09, 0x0a, 0x10, 0x11,
    0x12, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x01, 0x06, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0x00, 0x01, 0x04, 0x00, 0x07, 0x0c, 0x01, 0x02,
    0x18, 0x18, 0x18, 0x18, 0x09, 0x0a, 0x10, 0x11,
    0x12, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01,
    0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const char PROGMEM report_f2[] = {
    //Sixaxis
    /*0xf2, 0xff, 0xff, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //device bdaddr
                0x00, 0x03, 0x50, 0x81, 0xd8, 0x01,
    0x8a, 0x00, 0x00, 0x01, 0x64, 0x19, 0x01, 0x00,
    0x64, 0x00, 0x01, 0x90, 0x00, 0x19, 0xfe, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0xf2, 0xff, 0xff, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //device bdaddr
                0x00, 0x03, 0x50, 0x81, 0xd8, 0x01,
    0x8a, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01,
    0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const char PROGMEM report_f5[] = {
    //Sixaxis
    /*0x01, 0x00,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, //dummy PS3 bdaddr
    0x23, 0x1e, 0x00, 0x03, 0x50, 0x81, 0xd8, 0x01,
    0x8a, 0x00, 0x00, 0x01, 0x64, 0x19, 0x01, 0x00,
    0x64, 0x00, 0x01, 0x90, 0x00, 0x19, 0xfe, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0x01, 0x00,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, //dummy PS3 bdaddr
    0xff, 0xf7, 0x00, 0x03, 0x50, 0x81, 0xd8, 0x01,
    0x8a, 0x13, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x04,
    0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x01,
    0x02, 0x07, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const char PROGMEM report_ef[] = {
    //Sixaxis
    /*0x00, 0xef, 0x03, 0x00, 0x04, 0x03, 0x01, 0xb0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x05, 0x01, 0x92, 0x02, 0x02, 0x01,
    0x91, 0x02, 0x05, 0x01, 0x91, 0x02, 0x04, 0x00,
    0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0x00, 0xef, 0x04, 0x00, 0x07, 0x03, 0x01, 0xb0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x6b, 0x02, 0x68, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const char PROGMEM report_f8[] = {
    //Sixaxis
    /*0x00, 0x01, 0x00, 0x00, 0x07, 0x03, 0x01, 0xb0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x6b, 0x02, 0x68, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0x00, 0x01, 0x00, 0x00, 0x07, 0x03, 0x01, 0xb0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x6b, 0x02, 0x68, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const char PROGMEM report_f7[] = {
    //Sixaxis
    /*0x01, 0x00, 0x08, 0x03, 0xd2, 0x01, 0xee, 0xff,
    0x10, 0x02, 0x00, 0x03, 0x50, 0x81, 0xd8, 0x01,
    0x8a, 0x00, 0x00, 0x01, 0x64, 0x19, 0x01, 0x00,
    0x64, 0x00, 0x01, 0x90, 0x00, 0x19, 0xfe, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,*/

    //Dualshock 3
    0x01, 0x04, 0xc4, 0x02, 0xd6, 0x01, 0xee, 0xff,
    0x14, 0x13, 0x01, 0x02, 0xc4, 0x01, 0xd6, 0x00,
    0x00, 0x02, 0x02, 0x02, 0x00, 0x03, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x02, 0x62, 0x01, 0x02, 0x01,
    0x5e, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequestPS4(void)
{
  static unsigned char buffer[MAX_CONTROL_TRANSFER_SIZE];
  unsigned char len = 0;

  /* Handle HID Class specific requests */
  switch (USB_ControlRequest.bRequest)
  {
    case REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        if(USB_ControlRequest.wValue == 0x0303)
        {
          memcpy_P(buffer, buf03, sizeof(buf03));
          len = sizeof(buf03);
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(buffer, len);
          Endpoint_ClearOUT();
          ready = 1;
        }
        else if(USB_ControlRequest.wValue == 0x03f3)
        {
          memcpy_P(buffer, buff3, sizeof(buff3));
          len = sizeof(buff3);
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(buffer, len);
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03f1
              || USB_ControlRequest.wValue == 0x03f2)
        {
          spoofReply = 0;
          send_spoof_header();
          while(!spoofReply);
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(buf, spoofReplyLen);
          Endpoint_ClearOUT();
        }
      }
    break;
    case REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();

        if(USB_ControlRequest.wValue == 0x03f0)
        {
          send_spoof_header();
          Serial_SendData(buffer, USB_ControlRequest.wLength);
        }
      }
   
    break;
  }
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequestPS3(void)
{
  static unsigned char buffer[MAX_CONTROL_TRANSFER_SIZE];

  /* Handle HID Class specific requests */
  switch (USB_ControlRequest.bRequest)
  {
    case REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        if(reportType == REPORT_TYPE_FEATURE)
        {
          const void* feature = NULL;
          unsigned char len = 0;

          switch(reportId)
          {
            case 0x01:
              feature = report_01;
              len = sizeof(report_01);
            break;
            case 0xf2:
              feature = report_f2;
              len = sizeof(report_f2);
            break;
            case 0xf5:
              memcpy_P(buffer, report_f5, sizeof(buffer));
              if(reply == 0)
              {
                /*
                 * First request, tell that the bdaddr is not the one of the PS3.
                 */
                reply = 1;
              }
              else
              {
                /*
                 * Next requests, tell that the bdaddr is the one of the PS3.
                 */
                memcpy(buffer+2, masterBdaddr, 6);
              }
              len = sizeof(report_f5);
            break;
            case 0xef:
              memcpy_P(buffer, report_ef, sizeof(buffer));
              buffer[7] = byte_6_ef;
              len = sizeof(report_ef);
            break;
            case 0xf8:
              memcpy_P(buffer, report_f8, sizeof(buffer));
              buffer[7] = byte_6_ef;//necessary??
              len = sizeof(report_f8);
            break;
            case 0xf7:
              feature = report_f7;
              len = sizeof(report_f7);
            break;
            default:
              Serial_SendByte(BYTE_DEBUG);
              Serial_SendByte(BYTE_LEN_1_BYTE);
              Serial_SendByte(reportId);
            break;
          }
          if(feature)
          {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_PStream_LE(feature, USB_ControlRequest.wLength);
            Endpoint_ClearOUT();
          }
          else if(len)
          {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
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
            case 0xf5:
              memcpy(masterBdaddr, buffer+2, 6);
            break;
            case 0xef:
              byte_6_ef = buffer[6];
            break;
          }
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

#ifdef REPORT_NB_INFO
static unsigned char nbReports = 0;
static unsigned char info[] = { BYTE_DEBUG, BYTE_LEN_0_BYTE };
#endif

/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
 /* Select the IN Report Endpoint */
 Endpoint_SelectEndpoint(IN_EPNUM);

  if (((consoleModel == 1) && sendReport) || ((consoleModel == 0) && ready && sendReport))
  {
    /* Wait until the host is ready to accept another packet */
    while (!Endpoint_IsINReady()) {}

    /* Write IN Report Data */
    if(consoleModel == 0)
    {
      Endpoint_Write_Stream_LE(reportPS4, sizeof(reportPS4), NULL);
    }
    else
    {
      Endpoint_Write_Stream_LE(reportPS3, sizeof(reportPS3), NULL);
    }

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();

    sendReport = 0;

    if(consoleModel == 0) {
#ifdef REPORT_NB_INFO
      nbReports++;
      if(!nbReports) {
        Serial_SendData(info, sizeof(info));
      }
#endif
    }
  }
}


void EVENT_USB_Device_ControlRequest(void)
{
  if(consoleModel == 0){
    EVENT_USB_Device_ControlRequestPS3();
  }
  else
  {
    EVENT_USB_Device_ControlRequestPS4();
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

/** Function to read digital input to see if is high or low. Set to PIN 7. */
void readDeviceType(void)
{
  if(PIND & 0b10000000) {
    consoleModel = 1;
  }
}

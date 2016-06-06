/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)
      Based on code by Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *  Header file for emu.c.
 */

#ifndef _EMU_H_
#define _EMU_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <stdbool.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>
//#include <LUFA/Drivers/Board/Joystick.h>
//#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Drivers/Board/LEDs.h>

/* Macros: */
/** HID Class specific request to get the next HID report from the device. */
#define REQ_GetReport               0x01

/** HID Class specific request to send the next HID report to the device. */
#define REQ_SetReport               0x09

#define REQ_SetIdle                 0x0A

/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY        LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING     (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY           (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR           (LEDS_LED1 | LEDS_LED3)

/* Function Prototypes: */
void SetupHardware(void);
void HID_Task(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_UnhandledControlRequest(void);
//    void EVENT_USB_Device_StartOfFrame(void);

void ProcessLEDReport(uint8_t LEDReport);
void SendNextReport(void);
void ReceiveNextReport(void);

#endif

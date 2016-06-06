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
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

/* Includes: */
#include "LUFA/Drivers/USB/USB.h"

#include <avr/pgmspace.h>

/* Type Defines: */
/** Type define for the HID class specific HID descriptor, to describe the HID device's specifications. Refer to the HID
 *  specification for details on the structure elements.
 */
typedef struct
{
  USB_Descriptor_Header_t Header;
    
  uint16_t                HIDSpec;
  uint8_t                 CountryCode;

  uint8_t                 TotalReportDescriptors;

  uint8_t                 HIDReportType;
  uint16_t                HIDReportLength;
} USB_Descriptor_HID_t;

/** Type define for the data type used to store HID report descriptor elements. */
typedef uint8_t USB_Descriptor_HIDReport_Datatype_t;

/** Type define for the device configuration descriptor structure. This must be defined in the
 *  application code, as the configuration descriptor contains several sub-descriptors which
 *  vary between devices, and which describe the device's usage to the host.
 */
typedef struct
{
  USB_Descriptor_Configuration_Header_t Config;
  USB_Descriptor_Interface_t            Interface;
  USB_Descriptor_HID_t                  HID;
  USB_Descriptor_Endpoint_t             OutEndpoint;
  USB_Descriptor_Endpoint_t             InEndpoint;
} USB_Descriptor_Configuration_t;

/* Macros: */
/** Endpoint number of the HID reporting IN endpoint. */
#define IN_EPNUM            (ENDPOINT_DIR_IN | 2)

/** Endpoint number of the HID reporting OUT endpoint. */
#define OUT_EPNUM           (ENDPOINT_DIR_OUT | 1)

/** Size in bytes of the HID reporting IN and OUT endpoints. */
#define EPSIZE             64

/** Descriptor header type value, to indicate a HID class HID descriptor. */
#define DTYPE_HID                 0x21

/** Descriptor header type value, to indicate a HID class HID report descriptor. */
#define DTYPE_Report              0x22

#endif

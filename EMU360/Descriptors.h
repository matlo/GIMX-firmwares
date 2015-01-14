/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
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
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include "LUFA/Drivers/USB/USB.h"

		#include <avr/pgmspace.h>

	/* Type Defines: */
		/** Type define for the device configuration descriptor structure. This must be defined in the
		 *  application code, as the configuration descriptor contains several sub-descriptors which
		 *  vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;

			USB_Descriptor_Interface_t            Interface1;
			uint8_t                               UnkownDescriptor1[0x11];
	    USB_Descriptor_Endpoint_t             ReportINEndpoint11;
	    USB_Descriptor_Endpoint_t             ReportOUTEndpoint12;

	    USB_Descriptor_Interface_t            Interface2;
	    uint8_t                               UnkownDescriptor2[0x1b];
      USB_Descriptor_Endpoint_t             ReportINEndpoint21;
      USB_Descriptor_Endpoint_t             ReportOUTEndpoint22;
      USB_Descriptor_Endpoint_t             ReportINEndpoint23;
      USB_Descriptor_Endpoint_t             ReportOUTEndpoint24;

      USB_Descriptor_Interface_t            Interface3;
      uint8_t                               UnkownDescriptor3[0x09];
      USB_Descriptor_Endpoint_t             ReportINEndpoint31;

      USB_Descriptor_Interface_t            Interface4;
      uint8_t                               UnkownDescriptor4[0x06];

		} USB_Descriptor_Configuration_t;
					
	/* Macros: */
		/** Endpoint number of the HID reporting IN endpoint. */
		#define IN_EPNUM               (ENDPOINT_DIR_IN  | 1)

		/** Endpoint number of the HID reporting OUT endpoint. */
    #define OUT_EPNUM              (ENDPOINT_DIR_OUT | 2)
    
    /** Other endpoints */
    #define EPNUM21                (ENDPOINT_DIR_IN  | 3)
    #define EPNUM22                (ENDPOINT_DIR_OUT | 4)
    #define EPNUM23                (ENDPOINT_DIR_IN  | 5)
    #define EPNUM24                (ENDPOINT_DIR_OUT | 5)
    #define EPNUM31                (ENDPOINT_DIR_IN  | 6)
		
		/** Size in bytes of the HID reporting IN and OUT endpoints. */
		#define EPSIZE              32

#endif

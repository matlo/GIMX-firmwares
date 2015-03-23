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
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special 
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.  
 */

#include "Descriptors.h"

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
		
	.USBSpecification       = VERSION_BCD(02.00),
	.Class                  = 0xFF,
	.SubClass               = 0x47,
	.Protocol               = 0xd0,
				
	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
		
	.VendorID               = 0x045e,
	.ProductID              = 0x02d1,
	.ReleaseNumber          = 0x0101,
		
	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = 0x03,
		
	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 3,
				
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,
				
			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP),
			
			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(500)
		},
		
  .Interface00 =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = 0x00,
      .AlternateSetting       = 0x00,

      .TotalEndpoints         = 2,

      .Class                  = 0xff,
      .SubClass               = 0x47,
      .Protocol               = 0xd0,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .I00ReportOUTEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I00_OUT_EPNUM,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = INTERRUPT_EPSIZE,
      .PollingIntervalMS      = 0x04
    },

  .I00ReportINEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I00_IN_EPNUM,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = INTERRUPT_EPSIZE,
      .PollingIntervalMS      = 0x04
    },

  .Interface10 =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = 0x01,
      .AlternateSetting       = 0x00,

      .TotalEndpoints         = 0,

      .Class                  = 0xff,
      .SubClass               = 0x47,
      .Protocol               = 0xd0,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .Interface11 =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = 0x01,
      .AlternateSetting       = 0x01,

      .TotalEndpoints         = 2,

      .Class                  = 0xff,
      .SubClass               = 0x47,
      .Protocol               = 0xd0,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .I11ReportOUTEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I11_OUT_EPNUM,
      .Attributes             = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = ISOCHRONOUS_EPSIZE,
      .PollingIntervalMS      = 0x01
    },

  .I11ReportINEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I11_IN_EPNUM,
      .Attributes             = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = ISOCHRONOUS_EPSIZE,
      .PollingIntervalMS      = 0x01
    },

  .Interface20 =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = 0x02,
      .AlternateSetting       = 0x00,

      .TotalEndpoints         = 0,

      .Class                  = 0xff,
      .SubClass               = 0x47,
      .Protocol               = 0xd0,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .Interface21 =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = 0x02,
      .AlternateSetting       = 0x01,

      .TotalEndpoints         = 2,

      .Class                  = 0xff,
      .SubClass               = 0x47,
      .Protocol               = 0xd0,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .I21ReportOUTEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I21_OUT_EPNUM,
      .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = BULK_EPSIZE,
      .PollingIntervalMS      = 0x00
    },

  .I21ReportINEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I21_IN_EPNUM,
      .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = BULK_EPSIZE,
      .PollingIntervalMS      = 0x00
    }
};

const USB_Descriptor_String_t PROGMEM XboxString =
{
  .Header                 = {.Size = USB_STRING_LEN(88), .Type = DTYPE_String},

  .UnicodeString          = L"Xbox Security Method 3, Version 1.00, © 2005 Microsoft Corporation. All rights reserved."
};

const USB_Descriptor_String_t PROGMEM ModString =
{
  .Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},

  .UnicodeString          = L"MSFT100\x90"
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},
		
	.UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(9), .Type = DTYPE_String},
		
	.UnicodeString          = L"Microsoft"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(10), .Type = DTYPE_String},
		
	.UnicodeString          = L"Controller"
};

const USB_Descriptor_String_t PROGMEM SerialString =
{
  .Header                 = {.Size = USB_STRING_LEN(12), .Type = DTYPE_String},

  .UnicodeString          = L"7EED81401555"
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*    Address = NULL;
	uint16_t Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device: 
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration: 
			Address = (void*)&ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String: 
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = (void*)&ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02: 
					Address = (void*)&ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
          Address = (void*)&SerialString;
          Size    = pgm_read_byte(&SerialString.Header.Size);
          break;
				case 0x04:
          Address = (void*)&XboxString;
          Size    = pgm_read_byte(&XboxString.Header.Size);
          break;
        case 0xee:
          Address = (void*)&ModString;
          Size    = pgm_read_byte(&ModString.Header.Size);
          break;
			}
			break;
	}
	
	*DescriptorAddress = Address;
	return Size;
}

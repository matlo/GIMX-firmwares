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
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special 
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.  
 */

#include "Descriptors.h"

/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM Report[] =
{
    0x05, 0x01, //Usage Page (Desktop)
    0x09, 0x04, //Usage (Joystick)
    0xA1, 0x01, //Collection (Application)
    0x85, 0x01, //Report ID (1)
    0x09, 0x30, //Usage (X)
    0x09, 0x31, //Usage (Y)
    0x09, 0x32, //Usage (Z)
    0x09, 0x35, //Usage (Rz)
    0x15, 0x00, //Logical Minimum (0)
    0x26, 0xFF, 0x00, //Logical Maximum (255)
    0x75, 0x08, //Report Size (8)
    0x95, 0x04, //Report Count (4)
    0x81, 0x02, //Input (Variable)
    0x09, 0x39, //Usage (Hat Switch)
    0x15, 0x00, //Logical Minimum (0)
    0x25, 0x07, //Logical Maximum (7)
    0x35, 0x00, //Physical Minimum (0)
    0x46, 0x3B, 0x01, //Physical Maximum (315)
    0x65, 0x14, //Unit (Degrees)
    0x75, 0x04, //Report Size (4)
    0x95, 0x01, //Report Count (1)
    0x81, 0x42, //Input (Variable, Null State)
    0x65, 0x00, //Unit
    0x05, 0x09, //Usage Page (Button)
    0x19, 0x01, //Usage Minimum (1)
    0x29, 0x0E, //Usage Maximum (14)
    0x15, 0x00, //Logical Minimum (0)
    0x25, 0x01, //Logical Maximum (1)
    0x75, 0x01, //Report Size (1)
    0x95, 0x0E, //Report Count (14)
    0x81, 0x02, //Input (Variable)
    0x06, 0x00, 0xFF, //Usage Page (FF00h)
    0x09, 0x20, //Usage (20h)
    0x75, 0x06, //Report Size (6)
    0x95, 0x01, //Report Count (1)
    0x81, 0x02, //Input (Variable)
    0x05, 0x01, //Usage Page (Desktop)
    0x09, 0x33, //Usage (Rx)
    0x09, 0x34, //Usage (Ry)
    0x15, 0x00, //Logical Minimum (0)
    0x26, 0xFF, 0x00, //Logical Maximum (255)
    0x75, 0x08, //Report Size (8)
    0x95, 0x02, //Report Count (2)
    0x81, 0x02, //Input (Variable)
    0x06, 0x00, 0xFF, //Usage Page (FF00h)
    0x09, 0x21, //Usage (21h)
    0x95, 0x36, //Report Count (54)
    0x81, 0x02, //Input (Variable)
    0x85, 0x05, //Report ID (5)
    0x09, 0x22, //Usage (22h)
    0x95, 0x1F, //Report Count (31)
    0x91, 0x02, //Output (Variable)
    0x85, 0x03, //Report ID (3)
    0x0A, 0x21, 0x27, //Usage (2721h)
    0x95, 0x2F, //Report Count (47)
    0xB1, 0x02, //Feature (Variable)
    0xC0, //End Collection
    0x06, 0xF0, 0xFF, //Usage Page (FFF0h)
    0x09, 0x40, //Usage (40h)
    0xA1, 0x01, //Collection (Application)
    0x85, 0xF0, //Report ID (240)
    0x09, 0x47, //Usage (47h)
    0x95, 0x3F, //Report Count (63)
    0xB1, 0x02, //Feature (Variable)
    0x85, 0xF1, //Report ID (241)
    0x09, 0x48, //Usage (48h)
    0x95, 0x3F, //Report Count (63)
    0xB1, 0x02, //Feature (Variable)
    0x85, 0xF2, //Report ID (242)
    0x09, 0x49, //Usage (49h)
    0x95, 0x0F, //Report Count (15)
    0xB1, 0x02, //Feature (Variable)
    0x85, 0xF3, //Report ID (243)
    0x0A, 0x01, 0x47, //Usage (4701h)
    0x95, 0x07, //Report Count (7)
    0xB1, 0x02, //Feature (Variable)
    0xC0, //End Collection
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
		
	.USBSpecification       = 0x0100,//1.00
	.Class                  = 0x00,
	.SubClass               = 0x00,
	.Protocol               = 0x00,
				
	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
		
	.VendorID               = 0x046D,
	.ProductID              = 0xC260,
	.ReleaseNumber          = 0x8900,
		
	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,
		
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
			.TotalInterfaces        = 1,
				
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,
				
			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
			
			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(200)
		},
		
	.Interface = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,
			
			.TotalEndpoints         = 2,
				
			.Class                  = 0x03,
			.SubClass               = 0x00,
			.Protocol               = 0x00,
				
			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.HID =
		{  
			.Header                 = {.Size = sizeof(USB_Descriptor_HID_t), .Type = DTYPE_HID},
			
			.HIDSpec                = 0x0110,
			.CountryCode            = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType          = DTYPE_Report,
			.HIDReportLength        = sizeof(Report)
		},

  .InEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = IN_EPNUM,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = EPSIZE,
      .PollingIntervalMS      = 0x05
    },
		
	.OutEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = OUT_EPNUM,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = EPSIZE,
			.PollingIntervalMS      = 0x05
		}
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
	.Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},
		
	.UnicodeString          = L"Logitech"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(30), .Type = DTYPE_String},
		
	.UnicodeString          = L"G29 Driving Force Racing Wheel"
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
			}
			break;
		case DTYPE_HID:
			Address = (void*)&ConfigurationDescriptor.HID;
			Size    = sizeof(USB_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			Address = (void*)&Report;
			Size    = sizeof(Report);
			break;
	}
	
	*DescriptorAddress = Address;
	return Size;
}

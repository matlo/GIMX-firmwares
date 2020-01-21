/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

#define DTYPE_HID    0x21
#define DTYPE_Report 0x22

const uint8_t PROGMEM JoystickReport[] = {
        0x05, 0x01, /* Usage Page (Generic Desktop)                       */
        0x09, 0x04, /* Usage (Joystick)                                   */
        0xa1, 0x01, /* Collection (Application)                           */
        0xa1, 0x02, /*   Collection (Logical)                             */
        0x15, 0x00, /*     Logical Minimum (0)                            */
        0x26, 0xFF, 0xFF, /*     Logical Maximum (65535)                        */
        0x75, 0x10, /*     Report Size (16)                               */
        0x95, 0x04, /*     Report Count (4)                               */
        0x09, 0x30, /*     Usage (X)                                      */
        0x09, 0x31, /*     Usage (Y)                                      */
        0x09, 0x32, /*     Usage (Z)                                      */
        0x09, 0x35, /*     Usage (Rz)                                     */
        0x81, 0x82, /*     Input (Data, Variable, Absolute, Volatile)     */
        0x25, 0x07, /*     Logical Maximum (7)                            */
        0x46, 0x3B, 0x01, /*     Physical Maximum (315)                         */
        0x75, 0x04, /*     Report Size (4)                                */
        0x95, 0x01, /*     Report Count (1)                               */
        0x65, 0x14, /*     Unit (Eng Rot: Degree)                         */
        0x09, 0x39, /*     Usage (Hat Switch)                             */
        0x81, 0x42, /*     Input (Data,Var,Abs,NWrp,Lin,Pref,Null,Bit)    */
        0x75, 0x04, /*     Report Size (4)                                */
        0x95, 0x01, /*     Report Count (1)                               */
        0x05, 0xFF, /*     Usage Page (null)                              */
        0x81, 0x01, /*     Input (Constant)                               */
        0x75, 0x08, /*     Report Size (8)                                */
        0x95, 0x01, /*     Report Count (1)                               */
        0x05, 0xFF, /*     Usage Page (null)                              */
        0x81, 0x01, /*     Input (Constant)                               */
        0x15, 0x00, /*     Logical Minimum (0)                            */
        0x25, 0x01, /*     Logical Maximum (1)                            */
        0x75, 0x01, /*     Report Size (1)                                */
        0x95, 0x0D, /*     Report Count (13)                              */
        0x05, 0x09, /*     Usage Page (Button)                            */
        0x19, 0x01, /*     Usage Minimum (Button 1)                       */
        0x29, 0x0D, /*     Usage Maximum (Button 13)                      */
        0x81, 0x02, /*     Input (Data, Variable, Absolute)               */
        0x75, 0x03, /*     Report Size (3)                                */
        0x95, 0x01, /*     Report Count (1)                               */
        0x05, 0xFF, /*     Usage Page (null)                              */
        0x81, 0x01, /*     Input (Constant)                               */
        // Begin PS3 vendor page - Thanks UnoJoy!
        //  0x75, 0x08, // Report Size (8)
        //  0x06, 0x00, 0xff, // USAGE_PAGE (Vendor Specific)
        //  0x09, 0x20, // Unknown
        //  0x09, 0x21, // Unknown
        //  0x09, 0x22, // Unknown
        //  0x09, 0x23, // Unknown
        //  0x09, 0x24, // Unknown
        //  0x09, 0x25, // Unknown
        //  0x09, 0x26, // Unknown
        //  0x09, 0x27, // Unknown
        //  0x09, 0x28, // Unknown
        //  0x09, 0x29, // Unknown
        //  0x09, 0x2a, // Unknown
        //  0x09, 0x2b, // Unknown
        //  0x95, 0x0c, // REPORT_COUNT (12)
        //  0x81, 0x02, // INPUT (Data,Var,Abs)
        //  0x0a, 0x21, 0x26, // Unknown
        //  0x95, 0x08, // REPORT_COUNT (8)
        //  0xb1, 0x02, // FEATURE (Data,Var,Abs)*/
        0xc0, /*   End Collection                                   */
        0xc0 /* End Collection                                     */
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
        .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device },
        .USBSpecification = VERSION_BCD(01.10),
        .Class = USB_CSCP_NoDeviceClass,
        .SubClass = USB_CSCP_NoDeviceSubclass,
        .Protocol = USB_CSCP_NoDeviceProtocol,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x03EB,
        .ProductID = 0x2043,
        .ReleaseNumber = VERSION_BCD(00.01),
        .ManufacturerStrIndex = 0x01,
        .ProductStrIndex = 0x02,
        .SerialNumStrIndex = NO_DESCRIPTOR,
        .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const struct {
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t Interface;
    USB_HID_Descriptor_HID_t HID;
    USB_Descriptor_Endpoint_t InEndpoint;
} PROGMEM ConfigurationDescriptor = {
        .Config = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Config), .Type = DTYPE_Configuration },
                .TotalConfigurationSize = sizeof(ConfigurationDescriptor),
                .TotalInterfaces = 1,
                .ConfigurationNumber = 1,
                .ConfigurationStrIndex = NO_DESCRIPTOR,
                .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
        },
        .Interface = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x00,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 1,
                .Class = HID_CSCP_HIDClass,
                .SubClass = HID_CSCP_NonBootSubclass,
                .Protocol = HID_CSCP_NonBootProtocol,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .HID = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.HID), .Type = HID_DTYPE_HID },
                .HIDSpec = VERSION_BCD(01.11),
                .CountryCode = 0x00,
                .TotalReportDescriptors = 1,
                .HIDReportType = HID_DTYPE_Report,
                .HIDReportLength = sizeof(JoystickReport)
        },
        .InEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.InEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_IN_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_IN_SIZE,
                .PollingIntervalMS = ADAPTER_IN_INTERVAL
        }
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
        .Header = { .Size = USB_STRING_LEN(1), .Type = DTYPE_String },
        .UnicodeString = { LANGUAGE_ID_ENG }
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
        .Header = { .Size = USB_STRING_LEN(11), .Type = DTYPE_String },
        .UnicodeString = L"GIMX"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(18), .Type = DTYPE_String },
        .UnicodeString = L"Joystick"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, const void **const DescriptorAddress) {
    const uint8_t DescriptorType = (wValue >> 8);
    const uint8_t DescriptorNumber = (wValue & 0xFF);

    const void *Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;

    switch (DescriptorType) {
    case DTYPE_Device:
        Address = &DeviceDescriptor;
        Size = sizeof(DeviceDescriptor);
        break;
    case DTYPE_Configuration:
        Address = &ConfigurationDescriptor;
        Size = sizeof(ConfigurationDescriptor);
        break;
    case DTYPE_String:
        switch (DescriptorNumber) {
        case 0x00:
            Address = &LanguageString;
            Size = pgm_read_byte(&LanguageString.Header.Size);
            break;
        case 0x01:
            Address = &ManufacturerString;
            Size = pgm_read_byte(&ManufacturerString.Header.Size);
            break;
        case 0x02:
            Address = &ProductString;
            Size = pgm_read_byte(&ProductString.Header.Size);
            break;
        }
        break;
    case DTYPE_HID:
        Address = &ConfigurationDescriptor.HID;
        Size = sizeof(ConfigurationDescriptor.HID);
        break;
    case DTYPE_Report:
        Address = &JoystickReport;
        Size = sizeof(JoystickReport);
        break;
    }

    *DescriptorAddress = Address;
    return Size;
}


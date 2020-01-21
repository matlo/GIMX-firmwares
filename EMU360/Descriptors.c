/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
        .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device },
        .USBSpecification = VERSION_BCD(02.00),
        .Class = 0xFF,
        .SubClass = 0xFF,
        .Protocol = 0xFF,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x045e,
        .ProductID = 0x028e,
        .ReleaseNumber = 0x0110,
        .ManufacturerStrIndex = 0x01,
        .ProductStrIndex = 0x02,
        .SerialNumStrIndex = 0x03,
        .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const struct
 {
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t Interface1;
    uint8_t UnkownDescriptor1[0x11];
    USB_Descriptor_Endpoint_t ReportINEndpoint11;
    USB_Descriptor_Endpoint_t ReportOUTEndpoint12;
    USB_Descriptor_Interface_t Interface2;
    uint8_t UnkownDescriptor2[0x1b];
    USB_Descriptor_Endpoint_t ReportINEndpoint21;
    USB_Descriptor_Endpoint_t ReportOUTEndpoint22;
    USB_Descriptor_Endpoint_t ReportINEndpoint23;
    USB_Descriptor_Endpoint_t ReportOUTEndpoint24;
    USB_Descriptor_Interface_t Interface3;
    uint8_t UnkownDescriptor3[0x09];
    USB_Descriptor_Endpoint_t ReportINEndpoint31;
    USB_Descriptor_Interface_t Interface4;
    uint8_t UnkownDescriptor4[0x06];

} PROGMEM ConfigurationDescriptor = {
        .Config = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Config), .Type = DTYPE_Configuration },
                .TotalConfigurationSize = sizeof(ConfigurationDescriptor),
                .TotalInterfaces = 4,
                .ConfigurationNumber = 1,
                .ConfigurationStrIndex = NO_DESCRIPTOR,
                .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP),
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(500)
        },
        .Interface1 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface1), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x00,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 2,
                .Class = 0xff,
                .SubClass = 0x5d,
                .Protocol = 0x01,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .Interface2 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface2), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x01,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 4,
                .Class = 0xff,
                .SubClass = 0x5d,
                .Protocol = 0x03,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .Interface3 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface3), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x02,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 1,
                .Class = 0xff,
                .SubClass = 0x5d,
                .Protocol = 0x02,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .Interface4 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface4), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x03,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 0,
                .Class = 0xff,
                .SubClass = 0xfd,
                .Protocol = 0x13,
                .InterfaceStrIndex = 4
        },
        .UnkownDescriptor1 = {
                0x11, 0x21, 0x10, 0x01, 0x01, 0x25, 0x81, 0x14,
                0x03, 0x03, 0x03, 0x04, 0x13, 0x02, 0x08, 0x03,
                0x03
        },
        .UnkownDescriptor2 = {
                0x1b, 0x21, 0x00, 0x01, 0x01, 0x01, 0x83, 0x40,
                0x01, 0x04, 0x20, 0x16, 0x85, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x16, 0x05, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00
        },
        .UnkownDescriptor3 = {
                0x09, 0x21, 0x00, 0x01, 0x01, 0x22, 0x86, 0x07,
                0x00
        },
        .UnkownDescriptor4 = {
                0x06, 0x41, 0x00, 0x01, 0x01, 0x03
        },
        .ReportINEndpoint11 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportINEndpoint11), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_IN_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_IN_SIZE,
                .PollingIntervalMS = ADAPTER_IN_INTERVAL
        },
        .ReportOUTEndpoint12 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportOUTEndpoint12), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_OUT_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_OUT_SIZE,
                .PollingIntervalMS = ADAPTER_OUT_INTERVAL
        },
        .ReportINEndpoint21 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportINEndpoint21), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_IN  | 3),
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 32,
                .PollingIntervalMS = 0x02
        },
        .ReportOUTEndpoint22 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportOUTEndpoint22), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_OUT | 4),
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 32,
                .PollingIntervalMS = 0x04
        },
        .ReportINEndpoint23 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportINEndpoint23), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_IN  | 5),
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 32,
                .PollingIntervalMS = 0x40
        },
        .ReportOUTEndpoint24 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportOUTEndpoint24), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_OUT | 5),
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 32,
                .PollingIntervalMS = 0x10
        },
        .ReportINEndpoint31 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.ReportINEndpoint31), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_IN  | 6),
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 32,
                .PollingIntervalMS = 0x10
        },
};

const USB_Descriptor_String_t PROGMEM XboxString = {
        .Header = { .Size = USB_STRING_LEN(88), .Type = DTYPE_String },
        .UnicodeString = L"Xbox Security Method 3, Version 1.00, © 2005 Microsoft Corporation. All rights reserved."
};

const USB_Descriptor_String_t PROGMEM ModString = {
        .Header = { .Size = USB_STRING_LEN(8), .Type = DTYPE_String },
        .UnicodeString = L"MSFT100\x90"
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
        .Header = { .Size = USB_STRING_LEN(1), .Type = DTYPE_String },
        .UnicodeString = { LANGUAGE_ID_ENG }
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
        .Header = { .Size = USB_STRING_LEN(14), .Type = DTYPE_String },
        .UnicodeString = L"Microsoft Inc."
};

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(10), .Type = DTYPE_String },
        .UnicodeString = L"Controller"
};

const USB_Descriptor_String_t PROGMEM SerialString = {
        .Header = { .Size = USB_STRING_LEN(7), .Type = DTYPE_String },
        .UnicodeString = L"1008366"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, const void **const DescriptorAddress) {
    const uint8_t DescriptorType = (wValue >> 8);
    const uint8_t DescriptorNumber = (wValue & 0xFF);

    void *Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;

    switch (DescriptorType) {
    case DTYPE_Device:
        Address = (void*) &DeviceDescriptor;
        Size = sizeof(DeviceDescriptor);
        break;
    case DTYPE_Configuration:
        Address = (void*) &ConfigurationDescriptor;
        Size = sizeof(ConfigurationDescriptor);
        break;
    case DTYPE_String:
        switch (DescriptorNumber) {
        case 0x00:
            Address = (void*) &LanguageString;
            Size = pgm_read_byte(&LanguageString.Header.Size);
            break;
        case 0x01:
            Address = (void*) &ManufacturerString;
            Size = pgm_read_byte(&ManufacturerString.Header.Size);
            break;
        case 0x02:
            Address = (void*) &ProductString;
            Size = pgm_read_byte(&ProductString.Header.Size);
            break;
        case 0x03:
            Address = (void*) &SerialString;
            Size = pgm_read_byte(&SerialString.Header.Size);
            break;
        case 0x04:
            Address = (void*) &XboxString;
            Size = pgm_read_byte(&XboxString.Header.Size);
            break;
        case 0xee:
            Address = (void*) &ModString;
            Size = pgm_read_byte(&ModString.Header.Size);
            break;
        }
        break;
    }

    *DescriptorAddress = Address;
    return Size;
}

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
        .SubClass = 0x47,
        .Protocol = 0xd0,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x045e,
        .ProductID = 0x02d1,
        .ReleaseNumber = 0x0203,
        .ManufacturerStrIndex = 0x01,
        .ProductStrIndex = 0x02,
        .SerialNumStrIndex = 0x03,
        .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const struct {
    USB_Descriptor_Configuration_Header_t Config;

    USB_Descriptor_Interface_t Interface00;
    USB_Descriptor_Endpoint_t I00ReportOUTEndpoint;
    USB_Descriptor_Endpoint_t I00ReportINEndpoint;

    USB_Descriptor_Interface_t Interface10;
    USB_Descriptor_Interface_t Interface11;
    USB_Descriptor_Endpoint_t I11ReportOUTEndpoint;
    USB_Descriptor_Endpoint_t I11ReportINEndpoint;

    USB_Descriptor_Interface_t Interface20;
    USB_Descriptor_Interface_t Interface21;
    USB_Descriptor_Endpoint_t I21ReportOUTEndpoint;
    USB_Descriptor_Endpoint_t I21ReportINEndpoint;

} PROGMEM ConfigurationDescriptor = {
        .Config = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Config), .Type = DTYPE_Configuration },
                .TotalConfigurationSize = sizeof(ConfigurationDescriptor),
                .TotalInterfaces = 3,
                .ConfigurationNumber = 1,
                .ConfigurationStrIndex = NO_DESCRIPTOR,
                .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP),
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(500)
        },
        .Interface00 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface00), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x00,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 2,
                .Class = 0xff,
                .SubClass = 0x47,
                .Protocol = 0xd0,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .I00ReportOUTEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I00ReportOUTEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_OUT_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_OUT_SIZE,
                .PollingIntervalMS = ADAPTER_OUT_INTERVAL
        },
        .I00ReportINEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I00ReportINEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_IN_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_IN_SIZE,
                .PollingIntervalMS = ADAPTER_IN_INTERVAL
        },
        .Interface10 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface10), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x01,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 0,
                .Class = 0xff,
                .SubClass = 0x47,
                .Protocol = 0xd0,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .Interface11 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface11), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x01,
                .AlternateSetting = 0x01,
                .TotalEndpoints = 2,
                .Class = 0xff,
                .SubClass = 0x47,
                .Protocol = 0xd0,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .I11ReportOUTEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I11ReportOUTEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_OUT | 4),
                .Attributes = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 228,
                .PollingIntervalMS = 0x01
        },
        .I11ReportINEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I11ReportINEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_IN | 3),
                .Attributes = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 228,
                .PollingIntervalMS = 0x01
        },
        .Interface20 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface20), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x02,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 0,
                .Class = 0xff,
                .SubClass = 0x47,
                .Protocol = 0xd0,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .Interface21 = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface21), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x02,
                .AlternateSetting = 0x01,
                .TotalEndpoints = 2,
                .Class = 0xff,
                .SubClass = 0x47,
                .Protocol = 0xd0,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .I21ReportOUTEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I21ReportOUTEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_OUT | 6),
                .Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 64,
                .PollingIntervalMS = 0x00
        },
        .I21ReportINEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.I21ReportINEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = (ENDPOINT_DIR_IN | 5),
                .Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = 64,
                .PollingIntervalMS = 0x00
        }
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
        .Header = { .Size = USB_STRING_LEN(9), .Type = DTYPE_String },
        .UnicodeString = L"Microsoft"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(10), .Type = DTYPE_String },
        .UnicodeString = L"Controller"
};

const USB_Descriptor_String_t PROGMEM SerialString = {
        .Header = { .Size = USB_STRING_LEN(7), .Type = DTYPE_String },
        .UnicodeString = L"0000001"
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

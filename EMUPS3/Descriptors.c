/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

#define DTYPE_HID    0x21
#define DTYPE_Report 0x22

const USB_Descriptor_HIDReport_Datatype_t PROGMEM Report[] = {
        0x05, 0x01,
        0x09, 0x04,
        0xa1, 0x01,
        0xa1, 0x02,
        0x85, 0x01,
        0x75, 0x08,
        0x95, 0x01,
        0x15, 0x00,
        0x26, 0xff, 0x00,
        0x81, 0x03,
        0x75, 0x01,
        0x95, 0x13,
        0x15, 0x00,
        0x25, 0x01,
        0x35, 0x00,
        0x45, 0x01,
        0x05, 0x09,
        0x19, 0x01,
        0x29, 0x13,
        0x81, 0x02,
        0x75, 0x01,
        0x95, 0x0d,
        0x06, 0x00, 0xff,
        0x81, 0x03,
        0x15, 0x00,
        0x26, 0xff, 0x00,
        0x05, 0x01,
        0x09, 0x01,
        0xa1, 0x00,
        0x75, 0x08,
        0x95, 0x04,
        0x35, 0x00,
        0x46, 0xff, 0x00,
        0x09, 0x30,
        0x09, 0x31,
        0x09, 0x32,
        0x09, 0x35,
        0x81, 0x02,
        0xc0,
        0x05, 0x01,
        0x75, 0x08,
        0x95, 0x27,
        0x09, 0x01,
        0x81, 0x02,
        0x75, 0x08,
        0x95, 0x30,
        0x09, 0x01,
        0x91, 0x02,
        0x75, 0x08,
        0x95, 0x30,
        0x09, 0x01,
        0xb1, 0x02,
        0xc0,
        0xa1, 0x02,
        0x85, 0x02,
        0x75, 0x08,
        0x95, 0x30,
        0x09, 0x01,
        0xb1, 0x02,
        0xc0,
        0xa1, 0x02,
        0x85, 0xee,
        0x75, 0x08,
        0x95, 0x30,
        0x09, 0x01,
        0xb1, 0x02,
        0xc0,
        0xa1, 0x02,
        0x85, 0xef,
        0x75, 0x08,
        0x95, 0x30,
        0x09, 0x01,
        0xb1, 0x02,
        0xc0,
        0xc0
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
        .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device },
        .USBSpecification = VERSION_BCD(02.00),
        .Class = 0x00,
        .SubClass = 0x00,
        .Protocol = 0x00,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x054c,
        .ProductID = 0x0268,
        .ReleaseNumber = 0x0100,
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
    USB_Descriptor_Endpoint_t OutEndpoint;
} PROGMEM ConfigurationDescriptor = {
        .Config = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Config), .Type = DTYPE_Configuration },
                .TotalConfigurationSize = sizeof(ConfigurationDescriptor),
                .TotalInterfaces = 1,
                .ConfigurationNumber = 1,
                .ConfigurationStrIndex = NO_DESCRIPTOR,
                .ConfigAttributes = USB_CONFIG_ATTR_RESERVED,
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(500)
        },
        .Interface = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x00,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 2,
                .Class = 0x03,
                .SubClass = 0x00,
                .Protocol = 0x00,
                .InterfaceStrIndex = NO_DESCRIPTOR
        },
        .HID = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.HID), .Type = DTYPE_HID },
                .HIDSpec = 0x0111,
                .CountryCode = 0x00,
                .TotalReportDescriptors = 1,
                .HIDReportType = DTYPE_Report,
                .HIDReportLength = sizeof(Report)
        },
        .InEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.InEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_IN_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_IN_SIZE,
                .PollingIntervalMS = ADAPTER_IN_INTERVAL
        },
        .OutEndpoint = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.OutEndpoint), .Type = DTYPE_Endpoint },
                .EndpointAddress = ADAPTER_OUT_NUM,
                .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
                .EndpointSize = ADAPTER_OUT_SIZE,
                .PollingIntervalMS = ADAPTER_OUT_INTERVAL
        }
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
        .Header = { .Size = USB_STRING_LEN(1), .Type = DTYPE_String },
        .UnicodeString = { LANGUAGE_ID_ENG }
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
        .Header = { .Size = USB_STRING_LEN(5), .Type = DTYPE_String },
        .UnicodeString = L"Sony"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(26), .Type = DTYPE_String },
        .UnicodeString = L"PLAYSTATION(R)3 Controller"
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
        }
        break;
    case DTYPE_HID:
        Address = (void*) &ConfigurationDescriptor.HID;
        Size = sizeof(ConfigurationDescriptor.HID);
        break;
    case DTYPE_Report:
        Address = (void*) &Report;
        Size = sizeof(Report);
        break;
    }

    *DescriptorAddress = Address;
    return Size;
}

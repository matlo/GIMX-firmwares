/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

#define DTYPE_HID    0x21
#define DTYPE_Report 0x22

const uint8_t PROGMEM Report[] = {
        0x05, 0x01,    // Usage Page (Generic Desktop Ctrls)
        0x09, 0x04,    // Usage (Joystick)
        0xA1, 0x01,    // Collection (Application)
        0xA1, 0x02,    //   Collection (Logical)
        0x95, 0x01,    //   Report Count (1)
        0x75, 0x0A,    //   Report Size (10)
        0x15, 0x00,    //   Logical Minimum (0)
        0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
        0x35, 0x00,    //   Physical Minimum (0)
        0x46, 0xFF, 0x03,  //   Physical Maximum (1023)
        0x09, 0x30,    //   Usage (X)
        0x81, 0x02,    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x95, 0x06,    //   Report Count (6)
        0x75, 0x01,    //   Report Size (1)
        0x25, 0x01,    //   Logical Maximum (1)
        0x45, 0x01,    //   Physical Maximum (1)
        0x05, 0x09,    //   Usage Page (Button)
        0x19, 0x01,    //   Usage Minimum (0x01)
        0x29, 0x06,    //   Usage Maximum (0x06)
        0x81, 0x02,    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x95, 0x01,    //   Report Count (1)
        0x75, 0x08,    //   Report Size (8)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x46, 0xFF, 0x00,  //   Physical Maximum (255)
        //0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
        0x09, 0x01,    //   Usage (0x01)
        0x81, 0x02,    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x01,    //   Usage Page (Generic Desktop Ctrls)
        0x09, 0x31,    //   Usage (Y)
        0x81, 0x02,    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        //0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
        0x09, 0x01,    //   Usage (0x01)
        0x95, 0x03,    //   Report Count (3)
        0x81, 0x02,    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,        //   End Collection
        0xA1, 0x02,    //   Collection (Logical)
        0x09, 0x02,    //   Usage (0x02)
        0x95, 0x07,    //   Report Count (7)
        0x91, 0x02,    //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0xC0,        //   End Collection
        0xC0,        // End Collection
        };

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = { .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device }, .USBSpecification =
        VERSION_BCD(01.00), .Class = 0x00, .SubClass = 0x00, .Protocol = 0x00, .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE, .VendorID = 0x046d, .ProductID =
        0xc293, .ReleaseNumber = 0x0000, .ManufacturerStrIndex = 0x04, .ProductStrIndex = 0x20, .SerialNumStrIndex = NO_DESCRIPTOR, .NumberOfConfigurations =
        FIXED_NUM_CONFIGURATIONS };

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
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(80) },
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
                .HIDSpec = 0x0100,
                .CountryCode = 0x21,
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
        }, .OutEndpoint = {
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
        .Header = { .Size = USB_STRING_LEN(8), .Type = DTYPE_String },
        .UnicodeString = L"Logitech"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(17), .Type = DTYPE_String },
        .UnicodeString = L"Logitech GT Force"
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
        case 0x04:
            Address = (void*) &ManufacturerString;
            Size = pgm_read_byte(&ManufacturerString.Header.Size);
            break;
        case 0x20:
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

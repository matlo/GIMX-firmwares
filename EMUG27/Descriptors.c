/*
 Copyright (c) 2019 Mathieu Laurendeau
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

#define DTYPE_HID    0x21
#define DTYPE_Report 0x22

const uint8_t PROGMEM Report[] = {

//        0x05, 0x01, /*  Usage Page (Desktop),               */
//        0x09, 0x04, /*  Usage (Joystik),                    */
//        0xA1, 0x01, /*  Collection (Application),           */
//        0x14, /*      Logical Minimum (0),            */
//        0x25, 0x07, /*      Logical Maximum (7),            */
//        0x34, /*      Physical Minimum (0),           */
//        0x46, 0x3B, 0x01, /*      Physical Maximum (315),         */
//        0x65, 0x14, /*      Unit (Degrees),                 */
//        0x09, 0x39, /*      Usage (Hat Switch),             */
//        0x75, 0x04, /*      Report Size (4),                */
//        0x95, 0x01, /*      Report Count (1),               */
//        0x81, 0x42, /*      Input (Variable, Null State),   */
//        0x64, /*      Unit,                           */
//        0x25, 0x01, /*      Logical Maximum (1),            */
//        0x45, 0x01, /*      Physical Maximum (1),           */
//        0x05, 0x09, /*      Usage Page (Button),            */
//        0x19, 0x01, /*      Usage Minimum (01h),            */
//        0x29, 0x13, /*      Usage Maximum (13h),            */
//        0x75, 0x01, /*      Report Size (1),                */
//        0x95, 0x13, /*      Report Count (19),              */
//        0x81, 0x02, /*      Input (Variable),               */
//        0x06, 0x00, 0xFF, /*      Usage Page (FF00h),             */
//        0x09, 0x01, /*      Usage (01h),                    */
//        0x95, 0x03, /*      Report Count (3),               */
//        0x81, 0x02, /*      Input (Variable),               */
//        0x26, 0xFF, 0x3F, /*      Logical Maximum (16383),        */
//        0x46, 0xFF, 0x3F, /*      Physical Maximum (16383),       */
//        0x75, 0x0E, /*      Report Size (14),               */
//        0x95, 0x01, /*      Report Count (1),               */
//        0x05, 0x01, /*      Usage Page (Desktop),           */
//        0x09, 0x30, /*      Usage (X),                      */
//        0x81, 0x02, /*      Input (Variable),               */
//        0x26, 0xFF, 0x00, /*      Logical Maximum (255),          */
//        0x46, 0xFF, 0x00, /*      Physical Maximum (255),         */
//        0x75, 0x08, /*      Report Size (8),                */
//        0x95, 0x03, /*      Report Count (3),               */
//        0x09, 0x32, /*      Usage (Z),                      */
//        0x09, 0x35, /*      Usage (Rz),                     */
//        0x09, 0x31, /*      Usage (Y),                      */
//        0x81, 0x02, /*      Input (Variable),               */
//        0x06, 0x00, 0xFF, /*      Usage Page (FF00h),             */
//        0x09, 0x04, /*      Usage (04h),                    */
//        0x95, 0x03, /*      Report Count (3),               */
//        0x81, 0x02, /*      Input (Variable),               */
//        0x95, 0x07, /*      Report Count (7),               */
//        0x06, 0x00, 0xFF, /*      Usage Page (FF00h),             */
//        0x09, 0x02, /*      Usage (02h),                    */
//        0x91, 0x02, /*      Output (Variable),              */
//        0x95, 0x90, /*      Report Count (144),             */
//        0x09, 0x03, /*      Usage (03h),                    */
//        0xB1, 0x02, /*      Feature (Variable),             */
//        0xC0 /*  End Collection                      */
//         0x05, 0x01,
//         0x09, 0x04,
//         0xA1, 0x01,
//         0xA1, 0x02,
//         0x95, 0x01,
//         0x75, 0x0A,
//         0x15, 0x00,
//         0x26, 0xFF, 0x03,
//         0x35, 0x00,
//         0x46, 0xFF, 0x03,
//         0x09, 0x30,
//         0x81, 0x02,
//         0x95, 0x0C,
//         0x75, 0x01,
//         0x25, 0x01,
//         0x45, 0x01,
//         0x05, 0x09,
//         0x19, 0x01,
//         0x29, 0x0C,
//         0x81, 0x02,
//         0x95, 0x02,
//         0x06, 0x00, 0xFF,
//         0x09, 0x01,
//         0x81, 0x02,
//         0x05, 0x01,
//         0x09, 0x31,
//         0x26, 0xFF, 0x00,
//         0x46, 0xFF, 0x00,
//         0x95, 0x01,
//         0x75, 0x08,
//         0x81, 0x02,
//         0x25, 0x07,
//         0x46, 0x3B, 0x01,
//         0x75, 0x04,
//         0x65, 0x14,
//         0x09, 0x39,
//         0x81, 0x42,
//         0x75, 0x01,
//         0x95, 0x04,
//         0x65, 0x00,
//         0x06, 0x00, 0xFF,
//         0x09, 0x01,
//         0x25, 0x01,
//         0x45, 0x01,
//         0x81, 0x02,
//         0x95, 0x02,
//         0x75, 0x08,
//         0x26, 0xFF, 0x00,
//         0x46, 0xFF, 0x00,
//         0x09, 0x02,
//         0x81, 0x02,
//         0xC0,
//         0xA1, 0x02,
//         0x26, 0xFF, 0x00,
//         0x46, 0xFF, 0x00,
//         0x95, 0x07,
//         0x75, 0x08,
//         0x09, 0x03,
//         0x91, 0x02,
//         0x00, 0x00, 0x00,
//         0xC0,
//         0xC0
         0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0x15, 0x00, 0x25, 0x07, 0x35, 0x00, 0x46, 0x3B, 0x01, 0x65,
         0x14, 0x09, 0x39, 0x75, 0x04, 0x95, 0x01, 0x81, 0x42, 0x65, 0x00, 0x25, 0x01, 0x45, 0x01, 0x05,
         0x09, 0x19, 0x01, 0x29, 0x16, 0x75, 0x01, 0x95, 0x16, 0x81, 0x02, 0x26, 0xFF, 0x3F, 0x46, 0xFF,
         0x3F, 0x75, 0x0E, 0x95, 0x01, 0x05, 0x01, 0x09, 0x30, 0x81, 0x02, 0x26, 0xFF, 0x00, 0x46, 0xFF,
         0x00, 0x75, 0x08, 0x95, 0x03, 0x09, 0x32, 0x09, 0x35, 0x09, 0x31, 0x81, 0x02, 0x06, 0x00, 0xFF,
         0x09, 0x01, 0x95, 0x02, 0x81, 0x02, 0x95, 0x01, 0x75, 0x01, 0x25, 0x01, 0x45, 0x01, 0x05, 0x09,
         0x09, 0x17, 0x81, 0x02, 0x06, 0x00, 0xFF, 0x09, 0x01, 0x95, 0x07, 0x81, 0x02, 0x26, 0xFF, 0x00,
         0x46, 0xFF, 0x00, 0x06, 0x00, 0xFF, 0x09, 0x02, 0x95, 0x07, 0x75, 0x08, 0x91, 0x02, 0x95, 0x90,
         0x09, 0x03, 0xB1, 0x02, 0xC0,
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
        .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device },
        .USBSpecification = VERSION_BCD(02.00),
        .Class = 0x00,
        .SubClass = 0x00,
        .Protocol = 0x00,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x046d,
        .ProductID = 0xc29b,
        .ReleaseNumber = 0x1238,
        .ManufacturerStrIndex = NO_DESCRIPTOR,
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
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(98)
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

const USB_Descriptor_String_t PROGMEM ProductString = {
        .Header = { .Size = USB_STRING_LEN(16), .Type = DTYPE_String },
        .UnicodeString = L"G27 Racing Wheel"
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

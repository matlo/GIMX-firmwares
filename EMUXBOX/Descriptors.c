/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include <avr/pgmspace.h>
#include "LUFA/Drivers/USB/USB.h"
#include "Config/AdapterConfig.h"

#define DTYPE_HID    0x21
#define DTYPE_Report 0x22

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
        .Header = { .Size = sizeof(DeviceDescriptor), .Type = DTYPE_Device },
        .USBSpecification = 0x0110,
        .Class = 0x00,
        .SubClass = 0x00,
        .Protocol = 0x00,
        .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
        .VendorID = 0x045e,
        .ProductID = 0x0289,
        .ReleaseNumber = 0x0120,
        .ManufacturerStrIndex = 0x00,
        .ProductStrIndex = 0x00,
        .SerialNumStrIndex = 0x00,
        .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const struct {
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t Interface;
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
                .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
        },
        .Interface = {
                .Header = { .Size = sizeof(ConfigurationDescriptor.Interface), .Type = DTYPE_Interface },
                .InterfaceNumber = 0x00,
                .AlternateSetting = 0x00,
                .TotalEndpoints = 2,
                .Class = 0x58,
                .SubClass = 0x42,
                .Protocol = 0x00,
                .InterfaceStrIndex = NO_DESCRIPTOR
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

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, const void **const DescriptorAddress) {
    const uint8_t DescriptorType = (wValue >> 8);

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
    }

    *DescriptorAddress = Address;
    return Size;
}

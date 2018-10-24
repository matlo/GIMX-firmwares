
#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
  .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

  .USBSpecification       = 0x0200, //2.00
  .Class                  = 0xff,
  .SubClass               = 0xff,
  .Protocol               = 0xff,

  .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

  .VendorID               = 0x046D,
  .ProductID              = 0xC261,
  .ReleaseNumber          = 0x9601,

  .ManufacturerStrIndex   = 0x01,
  .ProductStrIndex        = 0x02,
  .SerialNumStrIndex      = 0x03,

  .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
  .Config =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

      .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
      .TotalInterfaces        = 1,

      .ConfigurationNumber    = 1,
      .ConfigurationStrIndex  = NO_DESCRIPTOR,

      .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP),

      .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
    },

  .Interface =
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

  .OutEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I00_OUT_EPNUM,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = INTERRUPT_EPSIZE,
      .PollingIntervalMS      = 0x04
    },

  .InEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = I00_IN_EPNUM,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = INTERRUPT_EPSIZE,
      .PollingIntervalMS      = 0x04
    },
};

const USB_Descriptor_String_t PROGMEM LanguageString =
{
  .Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

  .UnicodeString          = {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
  .Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},

  .UnicodeString          = L"Microsoft"
};

const USB_Descriptor_String_t PROGMEM ProductString =
{
  .Header                 = {.Size = USB_STRING_LEN(44), .Type = DTYPE_String},

  .UnicodeString          = L"G920 Driving Force Racing Wheel for Xbox One"
};

const USB_Descriptor_String_t PROGMEM SerialString =
{
  .Header                 = {.Size = USB_STRING_LEN(16), .Type = DTYPE_String},

  .UnicodeString          = L"0000123456789ABC"
};

const USB_Descriptor_String_t PROGMEM ConfigurationString =
{
  .Header                 = {.Size = USB_STRING_LEN(16), .Type = DTYPE_String},

  .UnicodeString          = L"U96.01_B0043"
};

const USB_Descriptor_String_t PROGMEM ModString =
{
  .Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},

  .UnicodeString          = L"MSFT100\x90"
};

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
          Address = (void*)&ConfigurationString;
          Size    = pgm_read_byte(&ConfigurationString.Header.Size);
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

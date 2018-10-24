
#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include "LUFA/Drivers/USB/USB.h"
#include <avr/pgmspace.h>

typedef struct
{
  USB_Descriptor_Configuration_Header_t Config;
  USB_Descriptor_Interface_t            Interface;
  USB_Descriptor_Endpoint_t             OutEndpoint;
  USB_Descriptor_Endpoint_t             InEndpoint;
} USB_Descriptor_Configuration_t;

#define I00_OUT_EPNUM              (ENDPOINT_DIR_OUT | 2)
#define I00_IN_EPNUM               (ENDPOINT_DIR_IN  | 1)

#define INTERRUPT_EPSIZE           64

#endif

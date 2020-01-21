/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

const uint8_t PROGMEM request144_index_4[] = {
        0x28, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x58, 0x47, 0x49, 0x50, 0x31, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void EVENT_USB_Device_ControlRequest(void) {

    if (USB_ControlRequest.bmRequestType & REQTYPE_VENDOR) {
        if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
            if (USB_ControlRequest.bRequest == 144) {
                uint8_t recipient = USB_ControlRequest.bmRequestType & 0x1F;
                if (recipient == REQREC_DEVICE) {
                    if (USB_ControlRequest.wIndex == 0x0004) {
                        Endpoint_ClearSETUP();
                        Endpoint_Write_Control_PStream_LE(request144_index_4, USB_ControlRequest.wLength);
                        Endpoint_ClearOUT();
                    }
                } else if (recipient == REQREC_INTERFACE) {
                    if (USB_ControlRequest.wIndex == 0x0005) {
                        Endpoint_ClearSETUP();
                        Endpoint_ClearOUT();
                    }
                }
            }
        }
    } else {
        if (USB_ControlRequest.bmRequestType & REQREC_INTERFACE) {
            if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
                if (USB_ControlRequest.bRequest == REQ_GetInterface) {
                    uint8_t data[1] = { 0x00 };
                    Endpoint_ClearSETUP();
                    Endpoint_Write_Control_Stream_LE(data, sizeof(data));
                    Endpoint_ClearOUT();
                }
            } else {
                if (USB_ControlRequest.bRequest == REQ_SetInterface) {
                    Endpoint_ClearSETUP();
                    // wLength is 0
                    Endpoint_ClearIN();
                }
            }
        }
    }
}

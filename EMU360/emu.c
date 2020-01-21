/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

static uint8_t response = 0;

void EVENT_USB_Device_ControlRequest(void) {

    static uint8_t buffer[MAX_CONTROL_TRANSFER_SIZE];

    if (USB_ControlRequest.bmRequestType & REQTYPE_VENDOR) {
        if (!spoof_initialized) {
            if (!(USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST)) {
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
                Endpoint_ClearIN();
            }
            spoofReply = 0;
            send_spoof_header();
            if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
                while (!spoofReply) {}
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(buf, spoofReplyLen);
                Endpoint_ClearOUT();
                if (USB_ControlRequest.wValue == 0x5c10) {
                    if (response) {
                        spoof_initialized = BYTE_STATUS_SPOOFED;
                    }
                    response = 1;
                } else if (USB_ControlRequest.wValue == 0x5b17) {
                }
            } else {
                Serial_SendData(buffer, USB_ControlRequest.wLength);
            }
        } else {
            if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
                if (USB_ControlRequest.wValue == 0x5b17) {
                    forceHardReset();
                }
                Endpoint_ClearSETUP();
                if (USB_ControlRequest.bRequest == 0x01) {
                    if (USB_ControlRequest.bmRequestType & REQREC_INTERFACE) {
                        uint8_t tmp[] = {
                                0x00, 0x14, 0xff, 0xf7, 0xff, 0xff, 0xc0, 0xff,
                                0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00
                        };
                        Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
                    } else {
                        uint8_t tmp[] = { 0x01, 0x00, 0x83, 0x66 };
                        Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
                    }
                } else if (USB_ControlRequest.bRequest == 0xa1) {
                    uint8_t tmp[] = { 0x01, 0x02 };
                    Endpoint_Write_Control_Stream_LE(tmp, sizeof(tmp));
                }
                Endpoint_ClearOUT();
            } else {
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
                Endpoint_ClearIN();
            }
        }
    }
}

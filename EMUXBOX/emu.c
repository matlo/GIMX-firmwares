/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

const uint8_t PROGMEM vendor1[] = {
        0x10, 0x42, 0x00, 0x01, 0x01, 0x02, 0x14, 0x06,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const uint8_t PROGMEM vendor2[] = {
        0x00, 0x06, 0xff, 0xff, 0xff, 0xff
};

const uint8_t PROGMEM vendor3[] = {
        0x00, 0x14, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
};

void EVENT_USB_Device_ControlRequest(void) {
    static uint8_t buffer[MAX_CONTROL_TRANSFER_SIZE];

    if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_INTERFACE)) {
        if (USB_ControlRequest.bRequest == 6) // vendor-defined request
                {
            if (USB_ControlRequest.wValue == 0x4200) {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor1, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
        } else if (USB_ControlRequest.bRequest == REQ_GetReport) {
            if (USB_ControlRequest.wValue == 0x0200) {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor2, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            } else if (USB_ControlRequest.wValue == 0x0100) {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(vendor3, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
        }
    } else if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
        if (USB_ControlRequest.bRequest == REQ_GetReport) {
            if (USB_ControlRequest.wValue == 0x0100) {
                report[12] = report[14] = report[16] = report[18] = 0x80;
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(report, USB_ControlRequest.wLength);
                Endpoint_ClearOUT();
            }
        }
    } else if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
        if (USB_ControlRequest.bRequest == REQ_SetReport) {
            if (USB_ControlRequest.wValue == 0x0200) {
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
                Endpoint_ClearIN();

                uint8_t length = USB_ControlRequest.wLength & 0xff;
                Serial_SendByte(BYTE_OUT_REPORT);
                Serial_SendByte(length);
                Serial_SendData(buffer, length);
            }
        }
    }
}

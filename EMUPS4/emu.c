/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

const uint8_t PROGMEM buff3[] = {
        0xF3, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00
};

const uint8_t PROGMEM buf03[] = {
        0x03, 0x21, 0x27, 0x04, 0x4d, 0x00, 0x2c, 0x56,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void EVENT_USB_Device_ControlRequest(void) {

    static uint8_t buffer[MAX_CONTROL_TRANSFER_SIZE];

    switch (USB_ControlRequest.bRequest) {
    case REQ_GetReport:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
            if (USB_ControlRequest.wValue == 0x0303) {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(buf03, sizeof(buf03));
                Endpoint_ClearOUT();
            } else if (USB_ControlRequest.wValue == 0x03f3) {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_PStream_LE(buff3, sizeof(buff3));
                Endpoint_ClearOUT();
            } else if (USB_ControlRequest.wValue == 0x03f1 || USB_ControlRequest.wValue == 0x03f2) {
                spoofReply = 0;
                send_spoof_header();
                while (!spoofReply) {}
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(buf, spoofReplyLen);
                Endpoint_ClearOUT();
            }
        }
        break;
    case REQ_SetReport:
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
            Endpoint_ClearSETUP();
            Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
            Endpoint_ClearIN();

            if (USB_ControlRequest.wValue == 0x03f0) {
                send_spoof_header();
                Serial_SendData(buffer, USB_ControlRequest.wLength);
            }
        }
        break;
    }
}

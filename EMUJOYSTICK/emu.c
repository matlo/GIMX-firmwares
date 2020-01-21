/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

/*
 * Series of bytes that appear in control packets right after the HID
 * descriptor is sent to the host. They where discovered by tracing output
 * from a Madcatz SF4 Joystick. Sending these bytes makes the PS button work.
 */
static const uint8_t PROGMEM magic_init_bytes[] = {
        0x21, 0x26, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00  // TODO MLA
};

void EVENT_USB_Device_ControlRequest(void) {
    switch (USB_ControlRequest.bRequest) {
    case HID_REQ_GetReport:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
            Endpoint_ClearSETUP();
            if (USB_ControlRequest.wValue == 0x0300) {
                Endpoint_Write_Control_PStream_LE(magic_init_bytes, sizeof(magic_init_bytes));
            } else {
                Endpoint_Write_Control_Stream_LE(report,  USB_ControlRequest.wLength);
            }

            Endpoint_ClearOUT();
        }
        break;
    }
}

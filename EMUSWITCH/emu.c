/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "adapter_switch.c"

void EVENT_USB_Device_ControlRequest(void) {
    // Switch doesn't use GetReport or SetReport

    switch (USB_ControlRequest.bRequest) {
        case HID_REQ_SetIdle: { // Difference between old firmware and this #1 : Old FW Sends a Broken Pipe status.
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();
                // Ignore idle period
            }
            break;
        }
            // No other HID requests are used
            // Standard control requests (such as SET_CONFIGURATION) are handled by LUFA
    }
}

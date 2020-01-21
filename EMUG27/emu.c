/*
 Copyright (c) 2019 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

#include "../adapter_common.c"

void EVENT_USB_Device_ControlRequest(void)
{
  static uint8_t buffer[MAX_CONTROL_TRANSFER_SIZE];

	switch (USB_ControlRequest.bRequest)
	{
    case REQ_SetIdle:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();
      }
      break;
    case REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        Serial_SendByte(BYTE_DEBUG);
        Serial_SendByte(2);
        Serial_SendByte(reportType);
        Serial_SendByte(reportId);
      }
      break;
    case REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();

        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        if(reportType == REPORT_TYPE_FEATURE)
        {
          switch(reportId)
          {
            default:
              Serial_SendByte(BYTE_DEBUG);
              Serial_SendByte(sizeof(USB_ControlRequest));
              Serial_SendData(&USB_ControlRequest, sizeof(USB_ControlRequest));
              break;
          }
        }
      }
      break;
  }
}

/*
 Copyright (c) 2013 Mathieu Laurendeau
 License: GPLv3
 */

#ifndef _ADAPTER_PROTOCOL_H_
#define _ADAPTER_PROTOCOL_H_

#define BYTE_NO_PACKET    0x00
#define BYTE_TYPE         0x11
#define BYTE_STATUS       0x22
#define BYTE_START        0x33
#define BYTE_CONTROL_DATA 0x44
#define BYTE_RESET        0x55
#define BYTE_DEBUG        0x99
#define BYTE_OUT_REPORT   0xee
#define BYTE_IN_REPORT    0xff

#define BYTE_TYPE_JOYSTICK   0x00
#define BYTE_TYPE_X360       0x01
#define BYTE_TYPE_SIXAXIS    0x02
#define BYTE_TYPE_PS2        0x03
#define BYTE_TYPE_XBOX       0x04
#define BYTE_TYPE_DS4        0x05
#define BYTE_TYPE_XBOXONE    0x06
#define BYTE_TYPE_T300RS_PS4 0x07
#define BYTE_TYPE_G27_PS3    0x08
#define BYTE_TYPE_G29_PS4    0x09
#define BYTE_TYPE_DF_PS2     0x0a
#define BYTE_TYPE_DFP_PS2    0x0b
#define BYTE_TYPE_GTF_PS2    0x0c

#define BYTE_STATUS_NSPOOFED 0x00
#define BYTE_STATUS_SPOOFED  0x01

#define BYTE_LEN_0_BYTE 0x00
#define BYTE_LEN_1_BYTE 0x01

#endif

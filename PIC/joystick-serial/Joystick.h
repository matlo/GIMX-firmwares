/*
  Redistributed under the GPLv3 licence.

  Code written by
    Ratt (http://forum.gimx.fr/viewtopic.php?p=3441#p3441).

  Code based on the work of
    Mathieu Laurendeau (mat.lau [at] laposte [dot] net)
*/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <usb.h>

//****** BEGIN CONFIG DESCRIPTOR LOOKUP TABLES ********
//since we can't make pointers to constants in certain pic16s, this is an offset table to find
//  a specific descriptor in the above table.

//NOTE: DO TO A LIMITATION OF THE CCS CODE, ALL HID INTERFACES MUST START AT 0 AND BE SEQUENTIAL
//      FOR EXAMPLE, IF YOU HAVE 2 HID INTERFACES THEY MUST BE INTERFACE 0 AND INTERFACE 1
#define USB_NUM_HID_INTERFACES   1

//the maximum number of interfaces seen on any config
//for example, if config 1 has 1 interface and config 2 has 2 interfaces you must define this as 2
#define USB_MAX_NUM_INTERFACES   1

//define how many interfaces there are per config.  [0] is the first config, etc.
const char USB_NUM_INTERFACES[USB_NUM_CONFIGURATIONS]={1};

//define where to find class descriptors
//first dimension is the config number
//second dimension specifies which interface
//last dimension specifies which class in this interface to get, but most will only have 1 class per interface
//if a class descriptor is not valid, set the value to 0xFFFF
const int16 USB_CLASS_DESCRIPTORS[USB_NUM_CONFIGURATIONS][1][1]=
{
//config 1
  //interface 0
     //class 1
     18          //indica el indice del array Config Descriptor[] donde comienza el class descriptor
};

//if a class has an extra descriptor not part of the config descriptor,
// this lookup table defines where to look for it in the const
// USB_CLASS_SPECIFIC_DESC[] array.
//first element is the config number (if your device has more than one config)
//second element is which interface number
//set element to 0xFFFF if this config/interface combo doesn't exist
const int16 USB_CLASS_SPECIFIC_DESC_LOOKUP[USB_NUM_CONFIGURATIONS][1] =
{
//config 1
  //interface 0
     0
};

const char USB_CLASS_SPECIFIC_DESC[] = {
  0x05, 0x01,          /* Usage Page (Generic Desktop)                       */
  0x09, 0x04,          /* Usage (Joystick)                                   */
  0xa1, 0x01,          /* Collection (Application)                           */
  0xa1, 0x02,          /*   Collection (Logical)                             */
  0x15, 0x00,          /*     Logical Minimum (0)                            */
  0x26, 0xFF, 0xFF,    /*     Logical Maximum (65535)                        */
  0x75, 0x10,          /*     Report Size (16)                               */
  0x95, 0x04,          /*     Report Count (4)                               */
  0x09, 0x30,          /*     Usage (X)                                      */
  0x09, 0x31,          /*     Usage (Y)                                      */
  0x09, 0x32,          /*     Usage (Z)                                      */
  0x09, 0x35,          /*     Usage (Rz)                                     */
  0x81, 0x82,          /*     Input (Data, Variable, Absolute, Volatile)     */
  0x25, 0x07,          /*     Logical Maximum (7)                            */
  0x46, 0x3B, 0x01,    /*     Physical Maximum (315)                         */
  0x75, 0x04,          /*     Report Size (4)                                */
  0x95, 0x01,          /*     Report Count (1)                               */
  0x65, 0x14,          /*     Unit (Eng Rot: Degree)                         */
  0x09, 0x39,          /*     Usage (Hat Switch)                             */
  0x81, 0x42,          /*     Input (Data,Var,Abs,NWrp,Lin,Pref,Null,Bit)    */
  0x75, 0x04,          /*     Report Size (4)                                */
  0x95, 0x01,          /*     Report Count (1)                               */
  0x05, 0xFF,          /*     Usage Page (null)                              */
  0x81, 0x01,          /*     Input (Constant)                               */
  0x75, 0x08,          /*     Report Size (8)                                */
  0x95, 0x01,          /*     Report Count (1)                               */
  0x05, 0xFF,          /*     Usage Page (null)                              */
  0x81, 0x01,          /*     Input (Constant)                               */
  0x15, 0x00,          /*     Logical Minimum (0)                            */
  0x25, 0x01,          /*     Logical Maximum (1)                            */
  0x75, 0x01,          /*     Report Size (1)                                */
  0x95, 0x0D,          /*     Report Count (13)                              */
  0x05, 0x09,          /*     Usage Page (Button)                            */
  0x19, 0x01,          /*     Usage Minimum (Button 1)                       */
  0x29, 0x0D,          /*     Usage Maximum (Button 13)                      */
  0x81, 0x02,          /*     Input (Data, Variable, Absolute)               */
  0x75, 0x03,          /*     Report Size (3)                                */
  0x95, 0x01,          /*     Report Count (1)                               */
  0x05, 0xFF,          /*     Usage Page (null)                              */
  0x81, 0x01,          /*     Input (Constant)                               */
  0xc0,                /*   End Collection                                   */
  0xc0                 /* End Collection                                     */
};

//if a class has an extra descriptor not part of the config descriptor,
// this lookup table defines the size of that descriptor.
//first element is the config number (if your device has more than one config)
//second element is which interface number
//set element to 0xFFFF if this config/interface combo doesn't exist
const int16 USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[USB_NUM_CONFIGURATIONS][1] =
{
//config 1
  //interface 0
     sizeof(USB_CLASS_SPECIFIC_DESC)    //longitud del reporte HID antes 32
};

unsigned char const Low_HID_Report_len = USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[0][0];
unsigned char const High_HID_Report_len = USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[0][0]>>8;

unsigned char const Low_HID_PACKET_SIZE = USB_EP1_RX_SIZE;
unsigned char const High_HID_PACKET_SIZE = USB_EP1_RX_SIZE>>8;

//////////////////////////////////////////////////////////////////
///
///   start device descriptors
///
//////////////////////////////////////////////////////////////////

const char USB_DEVICE_DESC[USB_DESC_DEVICE_LEN] ={
  //starts of with device configuration. only one possible
     USB_DESC_DEVICE_LEN, //the length of this report   ==1
     0x01, //the constant DEVICE (DEVICE 0x01)  ==2
     0x00,0x02, //usb version in bcd (2.0) ==3,4
     0x00, //class code ==5
     0x00, //subclass code ==6
     0x00, //protocol code ==7
     USB_MAX_EP0_PACKET_LENGTH, //max packet size for endpoint 0. (SLOW SPEED SPECIFIES 8) ==8
     0x34,0x12, //vendor id (0x04D8 is Microchip, or is it 0x0461 ??)
     0x01,0xAB, //product id   ==11,12  //don't use ffff says usb-by-example guy.  oops
     0x01,0x00, //device release number  ==13,14
     0x01, //index of string description of manufacturer. therefore we point to string_1 array (see below)  ==15
     0x02, //index of string descriptor of the product  ==16
     0x00, //index of string descriptor of serial number  ==17
     USB_NUM_CONFIGURATIONS  //number of possible configurations (1)  ==18
};

//////////////////////////////////////////////////////////////////
///
///   start config descriptor
///   right now we only support one configuration descriptor.
///   the config, interface, class, and endpoint goes into this array.
///
//////////////////////////////////////////////////////////////////

#DEFINE USB_TOTAL_CONFIG_LEN      41  //config+interface+class+endpoint+endpoint (2 endpoints)

const char USB_CONFIG_DESC[] = {
//IN ORDER TO COMPLY WITH WINDOWS HOSTS, THE ORDER OF THIS ARRAY MUST BE:
  //    config(s)
  //    interface(s)
  //    class(es)
  //    endpoint(s)

//config_descriptor for config index 1
     USB_DESC_CONFIG_LEN, //length of descriptor size          ==1
     USB_DESC_CONFIG_TYPE, //constant CONFIGURATION (CONFIGURATION 0x02)     ==2
     USB_TOTAL_CONFIG_LEN,0, //size of all data returned for this config      ==3,4
     1, //number of interfaces this device supports       ==5
     0x01, //identifier for this configuration.  (IF we had more than one configurations)      ==6
     0x00, //index of string descriptor for this configuration      ==7
     0xA0, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==8
     0x32, //maximum bus power required (maximum milliamperes/2)  (0x32 = 100mA)

//interface descriptor 1
     USB_DESC_INTERFACE_LEN, //length of descriptor      =10
     USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =11
     0x00, //number defining this interface (IF we had more than one interface)    ==12
     0x00, //alternate setting     ==13
     2, //number of endpoins, except 0 (pic167xx has 3, but we dont have to use all).       ==14
     0x03, //class code, 03 = HID     ==15
     0x00, //subclass code //boot     ==16
     0x00, //protocol code      ==17
     0x00, //index of string descriptor for interface      ==18

//class descriptor 1  (HID)
     USB_DESC_CLASS_LEN, //length of descriptor    ==19
     USB_DESC_CLASS_TYPE, //dscriptor type (0x21 == HID)      ==20
     0x01,0x01, //hid class release number (1.0) (try 1.10)      ==21,22
     0x00, //localized country code (0 = none)       ==23
     0x01, //number of hid class descrptors that follow (1)      ==24
     0x22, //report descriptor type (0x22 == HID)                ==25
     Low_HID_Report_len, High_HID_Report_len, //length of report descriptor            ==26,27

//endpoint descriptor
     USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==28
     USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==29
     0x81, //endpoint number and direction (0x81 = EP1 IN)       ==30
     0x03, //transfer type supported (0x03 is interrupt)         ==31
     Low_HID_PACKET_SIZE,High_HID_PACKET_SIZE, //maximum packet size supported                  ==32,33
     #if USB_USE_FULL_SPEED
     1,  //polling interval, in ms.  (cant be smaller than 10)      ==34
     #else
     10,  //polling interval, in ms.  (cant be smaller than 10)      ==34
     #endif

//endpoint descriptor
     USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==35
     USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==36
     0x01, //endpoint number and direction (0x01 = EP1 OUT)      ==37
     0x03, //transfer type supported (0x03 is interrupt)         ==38
     Low_HID_PACKET_SIZE,High_HID_PACKET_SIZE, //maximum packet size supported                  ==39,40
     #if USB_USE_FULL_SPEED
     1
     #else
     10 //polling interval, in ms.  (cant be smaller than 10)    ==41
     #endif
 };

 //////////////////////////////////////////////////////////////////
 ///
 ///  HID Report.  Tells HID driver how to handle and deal with
 ///  received data.  HID Reports can be extremely complex,
 ///  see HID specifcation for help on writing your own.
 ///
 ///  CCS example uses a vendor specified usage, that sends and
 ///  receives 2 absolute bytes ranging from 0 to 0xFF.
 ///
 //////////////////////////////////////////////////////////////////



#if (sizeof(USB_CONFIG_DESC) != USB_TOTAL_CONFIG_LEN)
    #error USB_TOTAL_CONFIG_LEN not defined correctly
#endif


//////////////////////////////////////////////////////////////////
///
///   start string descriptors
///   String 0 is a special language string, and must be defined.  People in U.S.A. can leave this alone.
///
///   You must define the length else get_next_string_character() will not see the string
///   Current code only supports 10 strings (0 thru 9)
///
//////////////////////////////////////////////////////////////////

//the offset of the starting location of each string.  offset[0] is the start of string 0, offset[1] is the start of string 1, etc.
char USB_STRING_DESC_OFFSET[]={0,4,12};

char const USB_STRING_DESC[]={
 //string 0
       4, //length of string index
       USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
       0x09,0x04,   //Microsoft Defined for US-English
 //string 1
       8, //length of string index
       USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
       'D',0,
       'I',0,
       'C',0,
 //string 2
       36, //length of string index
       USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
       'L',0,
       'O',0,
       'L',0,
       '-',0,
       'G',0,
       'I',0,
       'M',0,
       'X',0,
       ' ',0,
       'i',0,
       'z',0,
       ' ',0,
       'h',0,
       'A',0,
       'x',0,
       'N',0,
       'g',0,
};

#endif /* JOYSTICK_H_ */

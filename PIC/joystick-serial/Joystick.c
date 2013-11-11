/*
  Redistributed under the GPLv3 licence.

  Code written by
    Ratt (http://forum.gimx.fr/viewtopic.php?p=3441#p3441).

  Code based on the work of
    Mathieu Laurendeau (mat.lau [at] laposte [dot] net)
*/

#include "18F2550.h"

//FUSES
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN,NOPBADEN

//DELAY AND RS232
#use delay(clock=48000000)
#use rs232(UART1,baud=500000/*, xmit=PIN_C6, RCV=PIN_C7*/)

//USB DEFINES
#define  USB_HID_DEVICE    TRUE
#define  USB_EP1_TX_ENABLE USB_ENABLE_INTERRUPT
#define  USB_EP1_RX_ENABLE USB_ENABLE_INTERRUPT
#define  USB_EP1_TX_SIZE   64
#define  USB_EP1_RX_SIZE   64
#define  USB_USE_FULL_SPEED TRUE
#include <pic18_usb.h>
#include <Joystick.h> //PS3 PAD DESC.
#include <usb.c>

//BYTE COUNT
unsigned int i = 0;

//ARRAY WITH DEFAULT VALUES
unsigned int write[12] = {
0xFF, 0x7F,               //X_UPPNIBB, X_LOWNIBB,
0xFF, 0x7F,               //Y_UPPNIBB, Y_LOWNIBB,
0xED, 0x32,               //Z_UPPNIBB, Z_LOWNIBB,
0x52, 0x7B,               //Rz_UPPNIBB, Rz_LOWNIBB,
0x08, 0x00,               //POV,NULL
0x00, 0x00};               //BUTTONS

/*ff 7f ff 7f ed 32 52 7b 08 00 00 00*/

void main() {
   //USB INIT
   usb_init();
   usb_task();
   usb_wait_for_enumeration();

   //SERIAL INTS ENABLE
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_RDA);

   //MAIN LOOP
   while(1){
      usb_task();
      while(!usb_put_packet(1, write, 12, USB_DTS_TOGGLE));
   }
}

#INT_RDA //A NEW PACKAGE HAS ARRIVED!!! trollololo
void rda() {
   OUTPUT_toggle(PIN_D1);  //Debug stuff
   write[i]=getc();   //This three lines are copied
   i++;         //(and ported too) from matlo's AVR code,
   i%=12;         //and they seem to work just fine.
}

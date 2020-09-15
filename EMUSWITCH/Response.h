#ifndef JOYSTICK_RESPONSE_H
#define JOYSTICK_RESPONSE_H

#include "datatypes.h"
#include "Descriptors.h"
#include "EmulatedSPI.h"
#include <LUFA/Drivers/USB/USB.h>

inline void disable_rx_isr(void) {
    // Disable USART receive complete interrupt
    UCSR1B &= ~_BV(RXCIE1);
}

inline void enable_rx_isr(void) {
    // When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be
    // executed as long as the RXCn flag is set
    // Here, n=1
    UCSR1B |= _BV(RXCIE1);
}

void setup_response_manager(bool (*before_callback)(void), USB_ExtendedReport_t **ptr);
void process_OUT_report(uint8_t* ReportData, uint8_t ReportSize);
void send_IN_report(void);

#endif // JOYSTICK_RESPONSE_H

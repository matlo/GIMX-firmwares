/*
 Copyright (c) 2019 Mathieu Laurendeau
 License: GPLv3
 */

#include <stdint.h>
#include <stdbool.h>

#include <avr/wdt.h>
#include <avr/power.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Peripheral/Serial.h>

#include "../adapter_protocol.h"
#include "Config/AdapterConfig.h"

#ifndef ADAPTER_TYPE
#error ADAPTER_TYPE is not defined!
#endif

#ifndef ADAPTER_IN_NUM
#error ADAPTER_IN_NUM is not defined!
#endif

#ifndef ADAPTER_IN_SIZE
#error ADAPTER_IN_SIZE is not defined!
#endif

#ifndef ADAPTER_IN_INTERVAL
#error ADAPTER_IN_INTERVAL is not defined!
#endif

#ifdef ADAPTER_OUT_NUM
#ifndef ADAPTER_OUT_SIZE
#error ADAPTER_OUT_SIZE is not defined!
#endif

#ifndef ADAPTER_OUT_INTERVAL
#error ADAPTER_OUT_INTERVAL is not defined!
#endif
#endif

#define REQ_GetReport               0x01
#define REQ_SetReport               0x09
#define REQ_SetIdle                 0x0A

#define REPORT_TYPE_FEATURE 0x03

#define MAX_CONTROL_TRANSFER_SIZE 64

#define USART_BAUDRATE 5 // 500Kbps
#define USART_DOUBLE_SPEED true

const uint8_t version_major = 8;
const uint8_t version_minor = 0;

static uint8_t report[ADAPTER_IN_SIZE] = {};

static uint8_t buf[MAX_CONTROL_TRANSFER_SIZE];

static uint8_t *pdata;
static uint8_t i = 0;

/*
 * These variables are used in both the main and serial interrupt,
 * therefore they have to be declared as volatile.
 */
static volatile uint8_t sendReport = 0;
static volatile uint8_t reportLen = 0;
static volatile uint8_t started = 0;
static volatile uint8_t packet_type = 0;
static volatile uint8_t value_len = 0;
static volatile uint8_t spoofReply = 0;
static volatile uint8_t spoofReplyLen = 0;
static volatile uint8_t spoof_initialized = BYTE_STATUS_NSPOOFED;
static volatile uint16_t vid = 0;
static volatile uint16_t pid = 0;
static volatile uint8_t baudrate = USART_BAUDRATE;

void forceHardReset(void) {
    cli(); // disable interrupts
    wdt_enable(WDTO_15MS); // enable watchdog
    while (1) {} // wait for watchdog to reset processor
}

static inline bool Serial_WaitByte(volatile uint8_t * byte) {
    do {
      if (Serial_IsCharReceived()) {
          *byte = UDR1;
          return true;
        }
  } while (TCNT1 < 625);
  return false;
}

static inline void send_spoof_header(void) {
    Serial_SendByte(BYTE_CONTROL_DATA);
    if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST) {
        Serial_SendByte(sizeof(USB_ControlRequest));
    } else {
        Serial_SendByte(sizeof(USB_ControlRequest) + (USB_ControlRequest.wLength & 0xFF));
    }
    Serial_SendData(&USB_ControlRequest, sizeof(USB_ControlRequest));
}

static inline void handle_packet(void) {
    switch (packet_type) {
    case BYTE_TYPE:
        Serial_SendByte(BYTE_TYPE);
        Serial_SendByte(BYTE_LEN_1_BYTE);
        Serial_SendByte(ADAPTER_TYPE);
        break;
    case BYTE_STATUS:
        Serial_SendByte(BYTE_STATUS);
        Serial_SendByte(BYTE_LEN_1_BYTE);
        Serial_SendByte(spoof_initialized);
        break;
    case BYTE_START:
        Serial_SendByte(BYTE_START);
        Serial_SendByte(BYTE_LEN_1_BYTE);
        Serial_SendByte(spoof_initialized);
        started = 1;
        break;
    case BYTE_CONTROL_DATA:
        spoofReply = 1;
        spoofReplyLen = value_len;
        break;
    case BYTE_RESET:
        forceHardReset();
        break;
    case BYTE_IN_REPORT:
        sendReport = 1;
        reportLen = value_len;
        //no answer
        break;
    case BYTE_IDS:
        vid = buf[0] << 8 & buf[1];
        pid = buf[2] << 8 & buf[3];
        //no answer
        break;
    case BYTE_BAUDRATE:
        if (value_len > 0) {
          baudrate = buf[0];
          PORTD |= (1 << 3); // keep TX high while reconfiguring
          Serial_Disable();
          Serial_Init(baudrate * 100000U, true);
          PORTD &= ~(1 << 3); // release TX
          UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)
          //no answer
        } else {
          Serial_SendByte(BYTE_BAUDRATE);
          Serial_SendByte(1);
          Serial_SendByte(baudrate);
        }
        break;
    case BYTE_VERSION:
        Serial_SendByte(BYTE_VERSION);
        Serial_SendByte(2);
        Serial_SendByte(version_major);
        Serial_SendByte(version_minor);
        break;
    }
}
/*
ISR(USART1_RX_vect) {

    packet_type = UDR1;
    value_len = Serial_BlockingReceiveByte();
    pdata = (packet_type == BYTE_IN_REPORT) ? report : buf;
    while (i < value_len) {
        pdata[i++] = Serial_BlockingReceiveByte();
    }
    i = 0;
    handle_packet();
}*/

ISR(USART1_RX_vect) {

    packet_type = UDR1;
    /*
     * Reset packet reception timer: assume the maximum reception time for any packet is 10ms, and hard reset the
     * adapter if this time is exceeded. This helps recovering from a transmission error that could deadlock the
     * adapter in the ISR. This also helps auto-sensing baudrate as the USB to UART driver may accept a baudrate
     * setting that it does not actually support. An incorrect baudrate will result in a transmission issue that will
     * trigger a hard reset on firmware side, and a fallback to a lower baudrate on software side.
     */
    TCNT1 = 0;
    if (!Serial_WaitByte(&value_len)) {
        forceHardReset();
    }
    pdata = (packet_type == BYTE_IN_REPORT) ? report : buf;
    while (i < value_len) {
        if (!Serial_WaitByte(pdata + (i++))) {
            forceHardReset();
        }
    }
    i = 0;
    handle_packet();
}

void SetupHardware(void) {

    MCUSR = 0;
    wdt_disable();

    clock_prescale_set(clock_div_1);

    TCCR1B |= (1 << CS12); // Set up timer at FCPU / 256

    Serial_Init(baudrate * 100000U, true);

    UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)

    GlobalInterruptEnable();

    while (!started) {}

    USB_Init();
}

void EVENT_USB_Device_Connect(void) {
}

void EVENT_USB_Device_Disconnect(void) {
}

void EVENT_USB_Device_ConfigurationChanged(void) {

    Endpoint_ConfigureEndpoint(ADAPTER_IN_NUM, EP_TYPE_INTERRUPT, ADAPTER_IN_SIZE, 1);
#ifdef ADAPTER_OUT_NUM
    Endpoint_ConfigureEndpoint(ADAPTER_OUT_NUM, EP_TYPE_INTERRUPT, ADAPTER_OUT_SIZE, 1);
#endif
}

void SendNextReport(void) {

    Endpoint_SelectEndpoint(ADAPTER_IN_NUM);

    if (sendReport) {

        if (Endpoint_IsINReady()) {

            Endpoint_Write_Stream_LE(report, reportLen, NULL);
            sendReport = 0;
            Endpoint_ClearIN();
        }
    }
}

#ifdef ADAPTER_OUT_NUM
void ReceiveNextReport(void) {

    static struct {
        struct {
            uint8_t type;
            uint8_t length;
        } header;
        uint8_t buffer[ADAPTER_OUT_SIZE];
    } packet = { .header.type = BYTE_OUT_REPORT };

    Endpoint_SelectEndpoint(ADAPTER_OUT_NUM);

    if (Endpoint_IsOUTReceived()) {

        uint16_t length = 0;

        packet.header.length = 0;

        if (Endpoint_IsReadWriteAllowed()) {

            uint8_t ErrorCode = Endpoint_Read_Stream_LE(packet.buffer, sizeof(packet.buffer), &length);

            packet.header.length = (ErrorCode == ENDPOINT_RWSTREAM_NoError) ? sizeof(packet.buffer) : length;
        }

        Endpoint_ClearOUT();

        if (packet.header.length) {
            Serial_SendData(&packet, sizeof(packet.header) + packet.header.length);
        }
    }
}
#endif

void HID_Task(void) {

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    SendNextReport();

#ifdef ADAPTER_OUT_NUM
    ReceiveNextReport();
#endif
}

int main(void) {

    SetupHardware();

    while (1) {
        HID_Task();
        USB_USBTask();
    }
}

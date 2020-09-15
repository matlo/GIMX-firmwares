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
#include "Response.h"

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

static bool CALLBACK_beforeSend(void);
static USB_ExtendedReport_t *selectedReport; // Either &controllerReport or &idleReport
//static USB_StandardReport_t controllerReport;
static USB_ExtendedReport_t idleReport;

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
volatile uint16_t vid = 0;
volatile uint16_t pid = 0;
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
        vid = buf[0] << 8 | buf[1];
        pid = buf[2] << 8 | buf[3];
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

static void initialize_idle_report(USB_ExtendedReport_t *extendedReport) {
    memset(extendedReport, 0, sizeof(USB_ExtendedReport_t));

    USB_StandardReport_t *standardReport = &(extendedReport->standardReport);
    standardReport->connection_info = 1; // Pro Controller + USB connected
    standardReport->battery_level = BATTERY_FULL | BATTERY_CHARGING;

    /*standardReport->button_y = false;
    standardReport->button_x = false;
    standardReport->button_b = false;
    standardReport->button_a = false;
    standardReport->button_right_sr = false;
    standardReport->button_right_sl = false;
    standardReport->button_r = false;
    standardReport->button_zr = false;
    standardReport->button_minus = false;
    standardReport->button_plus = false;
    standardReport->button_thumb_r = false;
    standardReport->button_thumb_l = false;
    standardReport->button_home = false;
    standardReport->button_capture = false;
    standardReport->dummy = 0;
    standardReport->charging_grip = false;
    standardReport->dpad_down = false;
    standardReport->dpad_up = false;
    standardReport->dpad_right = false;
    standardReport->dpad_left = false;
    standardReport->button_left_sr = false;
    standardReport->button_left_sl = false;
    standardReport->button_l = false;
    standardReport->button_zl = false;*/
    standardReport->charging_grip = true;

    // Left stick
    uint16_t lx = 0x0800;
    uint16_t ly = 0x0800;
    standardReport->analog[0] = lx & 0xFF;
    standardReport->analog[1] = ((ly & 0x0F) << 4) | ((lx & 0xF00) >> 8);
    standardReport->analog[2] = (ly & 0xFF0) >> 4;

    // Right stick
    uint16_t rx = 0x0800;
    uint16_t ry = 0x0800;
    standardReport->analog[3] = rx & 0xFF;
    standardReport->analog[4] = ((ry & 0x0F) << 4) | ((rx & 0xF00) >> 8);
    standardReport->analog[5] = (ry & 0xFF0) >> 4;

    standardReport->vibrator_input_report = 0x0c;
}

static bool CALLBACK_beforeSend() {
    if (sendReport)
    {
        selectedReport = (USB_ExtendedReport_t *)&report[0];
        return true;
    }
    else
    {
        selectedReport = &idleReport;
        return false;
    }
}

void SendNextReport(void) {

    // We'll then move on to the IN endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
    // We first check to see if the host is ready to accept data.
    if (Endpoint_IsINReady()) {
        // Received IN interrupt. Switch wants a new packet.
        send_IN_report();
    }
}

void ReceiveNextReport(void) {
    // We'll start with the OUT endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
    // We'll check to see if we received something on the OUT endpoint.
    if (Endpoint_IsOUTReceived()) {
        // Messages from Switch
        static uint8_t switchResponseBuffer[JOYSTICK_EPSIZE];
        // Clear input buffer before every read
        memset(switchResponseBuffer, 0, sizeof(switchResponseBuffer));

        uint8_t *ReportData = switchResponseBuffer;
        uint16_t ReportSize = 0;
        // If we received something, and the packet has data, we'll store it.
        while (Endpoint_IsReadWriteAllowed()) {
            uint8_t b = Endpoint_Read_8();
            if (ReportSize < sizeof(switchResponseBuffer)) {
                // avoid over filling of the buffer
                *ReportData = b;
                ReportSize++;
                ReportData++;
            }
        }

        // We acknowledge an OUT packet on this endpoint.
        Endpoint_ClearOUT();

        // At this point, we can react to this data.
        process_OUT_report(switchResponseBuffer, ReportSize);
    }
}

void HID_Task(void) {

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    ReceiveNextReport();

    SendNextReport();
}

int main(void) {

    SetupHardware();
    // Assign default controller state values (no buttons pressed and centered sticks)
    //initialize_idle_report(&controllerReport); // Will be populated later with values received from UART
    initialize_idle_report(&idleReport); // Idle report (no buttons pressed)
    selectedReport = &idleReport; // Use idle report until data is received from UART

    setup_response_manager(CALLBACK_beforeSend, &selectedReport);
    while (1) {
        HID_Task();
        USB_USBTask();
    }
}

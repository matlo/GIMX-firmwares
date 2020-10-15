/*
 * my_data_receive.c
 *
 * Created: 2020/10/14 10:24
 * Author : Eric Hsu
 */

#include "../include/my_data_receive.h"

#define START_SIGNAL_MODE 1 // Standard mode -> 0; Simplified mode -> 1
#define CHECKSUM_MODE 1     // W/ checksum -> 0; W/O checksum -> 1
#define DATA_ROW 18         // Data length (ID + CMD + Len + Data(x...))
#define DATA_COL 12         // Command numbers

uint8_t rx_step, rx_length, rx_cmd = 0;
uint8_t data[DATA_ROW][DATA_COL] = {0};
uint8_t data_buffer[DATA_ROW] = {0};
uint8_t checksum_receive, data_counter = 0;

uint8_t data_event(uint8_t byte) {
    uint8_t flag = 0;

    // RX START Signal Check
#if START_SIGNAL_MODE == 0
    if ((rx_step == 0) && (byte == 0xD7)) {// Start signal 1
        rx_step = 1;
        flag = 1;
    }

    else if ((rx_step == 1) && (byte == 0xE5)) {// Start signal 2
        rx_step = 2;
        flag = 1;
    }

    else if ((rx_step == 2) && (byte == 0x61)) {// Start signal 3
        rx_step = 3;
        flag = 1;
    }

    // ID of RX Signal
    else if ((rx_step == 3) && (byte == 0x01)) {
        data_buffer[0]= byte;
        rx_step = 4;
        flag = 1;
    }

#elif START_SIGNAL_MODE == 1
    if ((rx_step == 0) && (byte == 0x55)) {
        data_buffer[0]= byte;
        rx_step = 4;
        flag = 1;
    }
#endif

    // CMD of I2C RX Signal
    else if (rx_step == 4) {
        rx_cmd = byte;
        data_buffer[1]= rx_cmd;
        rx_step = 5;
        flag = 1;
    }

    // LEN of I2C RX Signal
    else if (rx_step == 5) {
        rx_length = byte;
        data_buffer[2]= byte;
        rx_step = 6;
        data_counter = 0;
        flag = 1;
    }

    // DATA of I2C RX Signal
    else if ((rx_step == (6 + data_counter)) && (data_counter < rx_length)) {
        data_buffer[3 + data_counter] = byte;
        data_counter++;
        rx_step++;
        flag = 1;
    }

#if CHECKSUM_MODE == 0
    // Checksum of I2C RX Signal
    else if (rx_step == (6 + rx_length)) {
        uint8_t checksum_calculation = 0;

        checksum_receive = byte;

        uint8_t i = 0;
        for (i = 0; i < (rx_length + 3); i++) {  // CHKSUM= ID + CMD + Len + Data(x...)
            checksum_calculation = checksum_calculation + data_buffer[i];
        }

        if (checksum_calculation == checksum_receive) {
            uint8_t i = 0;
            for (i = 0; i < (rx_length + 3); i++) {
                data[rx_cmd][i] = data_buffer[i];
            }
            flag = 1;
        }
        else {
            printf("Checksum Error!");  // For debug
        }
        rx_step = 0;
    }
#elif CHECKSUM_MODE == 1
    // End of RX Receive
    if (rx_step == (6 + rx_length)) {
        uint8_t i = 0;
        for (i = 0; i < (rx_length + 3); i++) {
            data[rx_cmd][i] = data_buffer[i];
        }
        rx_step = 0;
        flag = 1;
    }
#endif
    return flag;
}

uint8_t get_data(uint8_t cmd,uint8_t num) {
    return data[cmd][2 + num];
}

/*
 * Wire.c
 *
 * Created: 2020/10/7 15:48
 * Author : TW019032
 */

#include "../include/Wire.h"
#include "../include/twi.h"

// Initialize Variables //////////////////////////////////////////////////

uint8_t rxBuffer[BUFFER_LENGTH];
uint8_t rxBufferIndex = 0;
uint8_t rxBufferLength = 0;

uint8_t txAddress = 0;
uint8_t txBuffer[BUFFER_LENGTH];
uint8_t txBufferIndex = 0;
uint8_t txBufferLength = 0;

uint8_t transmitting = 0;
void (*user_onRequest)(void);
void (*user_onReceive)(int);


// Methods //////////////////////////////////////////////////////////////

void begin(int address) {
    rxBufferIndex = 0;
    rxBufferLength = 0;

    txBufferIndex = 0;
    txBufferLength = 0;

    twi_init();
    twi_attachSlaveTxEvent(onRequestService); // default callback must exist
    twi_attachSlaveRxEvent(onReceiveService); // default callback must exist

    twi_setAddress((uint8_t)address);
}

void end(void)
{
  twi_disable();
}

void setClock(uint32_t clock)
{
  twi_setFrequency(clock);
}

uint8_t requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
    if (isize > 0) {
        // send internal address; this mode allows sending a repeated start to access
        // some devices' internal registers. This function is executed by the hardware
        // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)

        beginTransmission(address);

        // the maximum size of internal address is 3 bytes
        if (isize > 3){
            isize = 3;
        }

        // write internal register address - most significant byte first
        while (isize-- > 0)
            write_data((uint8_t)(iaddress >> (isize*8)));
        endTransmission(0x00);
    }

    // clamp to buffer length
    if(quantity > BUFFER_LENGTH){
        quantity = BUFFER_LENGTH;
    }
    // perform blocking read into buffer
    uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
    // set rx buffer iterator vars
    rxBufferIndex = 0;
    rxBufferLength = read;

    return read;
}

void beginTransmission(int address) {
    // indicate that we are transmitting
    transmitting = 1;
    // set address of targeted slave
    txAddress = (uint8_t)address;
    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
}

uint8_t endTransmission(uint8_t sendStop) {

    // transmit buffer (blocking)
    uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
    // indicate that we are done transmitting
    transmitting = 0;
    return ret;
}

int write_error;
void setWriteError(int err) {
    write_error = err;
}

unsigned int write_data(uint8_t data) {
    if(transmitting){
        // in master transmitter mode
        // don't bother if buffer is full
        if(txBufferLength >= BUFFER_LENGTH){
            setWriteError(0x01);
            return 0;
        }
        // put byte in tx buffer
        txBuffer[txBufferIndex] = data;
        ++txBufferIndex;
        // update amount in buffer
        txBufferLength = txBufferIndex;
        }else{
        // in slave send mode
        // reply to master
        twi_transmit(&data, 1);
    }
    return 1;
};

unsigned int write_list(const uint8_t *data, unsigned int quantity) {
    if(transmitting){
        // in master transmitter mode
        for(unsigned int i = 0; i < quantity; ++i){
            write_data(data[i]);
        }
        }else{
        // in slave send mode
        // reply to master
        twi_transmit(data, quantity);
    }
    return quantity;
}

int available(void) {
    return rxBufferLength - rxBufferIndex;
}

int read(void) {
    int value = -1;

    // get each successive byte on each call
    if(rxBufferIndex < rxBufferLength){
        value = rxBuffer[rxBufferIndex];
        ++rxBufferIndex;
    }

    return value;
}

int peek(void)
{
  int value = -1;

  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void flush(void)
{
  // XXX: to be implemented.
}

// behind the scenes function that is called when data is received
void onReceiveService(uint8_t* inBytes, int numBytes) {
    // don't bother if user hasn't registered a callback
    if(!user_onReceive) {
        return;
    }
    // don't bother if rx buffer is in use by a master requestFrom() op
    // i know this drops data, but it allows for slight stupidity
    // meaning, they may not have read all the master requestFrom() data yet
    if(rxBufferIndex < rxBufferLength) {
        return;
    }
    // copy twi rx buffer into local read buffer
    // this enables new reads to happen in parallel
    for(uint8_t i = 0; i < numBytes; ++i) {
        rxBuffer[i] = inBytes[i];
    }
    // set rx iterator vars
    rxBufferIndex = 0;
    rxBufferLength = numBytes;
    // alert user program
    user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void onRequestService(void) {
    // don't bother if user hasn't registered a callback
    if(!user_onRequest) {
        return;
    }
    // reset tx buffer iterator vars
    // !!! this will kill any pending pre-master sendTo() activity
    txBufferIndex = 0;
    txBufferLength = 0;
    // alert user program
    user_onRequest();
}

void onReceive( void (*function)(int) ) {
    user_onReceive = function;
}

// sets function called on slave read
void onRequest( void (*function)(void) ) {
    user_onRequest = function;
}
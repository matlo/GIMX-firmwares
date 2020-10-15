/*
 * Wire.h
 *
 * Created: 2020/10/7 15:48
 * Author : TW019032
 */

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>

#define BUFFER_LENGTH 32

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

void begin(int);
void end(void);
void setClock(uint32_t);
uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
void beginTransmission(int);
uint8_t endTransmission(uint8_t);
void setWriteError(int);
unsigned int write_data(uint8_t );
unsigned int write_list(const uint8_t *, unsigned int);
int available(void);
int read(void);
int peek(void);
void flush(void);
void onRequestService(void);
void onReceiveService(uint8_t*, int);
void onReceive( void (*)(int) );
void onRequest( void (*)(void) );

#endif
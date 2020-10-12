#ifndef twi_h
#define twi_h

  #include <inttypes.h>

  //#define ATMEGA8

	/* Define SCL clock frequency */
  #ifndef SCL_CLOCK
  #define SCL_CLOCK 400000L
  #endif  
  
  #ifndef F_CPU
  #define F_CPU 16000000UL
  #endif
  
  #include <util/delay.h>

  #ifndef TWI_BUFFER_LENGTH
  #define TWI_BUFFER_LENGTH 32
  #endif

  #define TWI_READY 0
  #define TWI_MRX   1
  #define TWI_MTX   2
  #define TWI_SRX   3
  #define TWI_STX   4
  
  void twi_init(void);
  void twi_disable(void);
  void twi_setAddress(uint8_t);
  void twi_setFrequency(uint32_t);
  uint8_t twi_readFrom(uint8_t, uint8_t*, uint8_t, uint8_t);
  uint8_t twi_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t);
  uint8_t twi_transmit(const uint8_t*, uint8_t);
  void twi_attachSlaveRxEvent( void (*)(uint8_t*, int) );
  void twi_attachSlaveTxEvent( void (*)(void) );
  void twi_reply(uint8_t);
  void twi_stop(void);
  void twi_releaseBus(void);

#endif


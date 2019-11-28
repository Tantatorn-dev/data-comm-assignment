#include <Arduino.h>
#include <FastCRC.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include <Wire.h>

#include <TEA5767Radio.h>

class FM_Rx
{
  public:
    FM_Rx(float freq);
    int receiveFM(unsigned long timeout = 2000);
    int receiveFrame(uint8_t *buffer, uint8_t startByte, uint8_t maxlen, unsigned long timeout = 2000);
    int receiveAck(unsigned long timeout = 2000);

  private:
    TEA5767Radio radio = TEA5767Radio();
    FastCRC8 CRC8;

    int8_t zone(uint16_t);
};

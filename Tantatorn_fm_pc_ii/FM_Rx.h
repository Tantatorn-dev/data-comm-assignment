#include <Arduino.h>

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
    FM_Rx();
    char receiveFM();
    int8_t zone(uint16_t);

private:
    TEA5767Radio radio = TEA5767Radio();

    int prev = 0;
    int check2 = false;
    int count = 0;
    uint32_t startFreq = 0;
    char data = 0;
    uint8_t bitC = 0;

};
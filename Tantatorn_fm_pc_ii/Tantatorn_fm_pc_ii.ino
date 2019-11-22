// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include <Wire.h>

#include <TEA5767Radio.h>
TEA5767Radio radio = TEA5767Radio();

#define r_slope 20
void setup() {
  // put your setup code here, to run once:
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  Serial.begin(115200);
  Serial.flush();

  Wire.begin();
  radio.setFrequency(90.04);
}

// Global variables

int prev = 0;
int check2 = false;
int count = 0;

uint32_t startFreq = 0;

char data = 0;
uint8_t bitC = 0;


void loop() {
  receiveFM();
}


// user defined functions

void receiveFM(){
  int8_t tmpZone = zone(analogRead(A2));
  if (tmpZone != prev) {
    if(tmpZone == 1 && prev != 1) {
      if (check2 == false) {
        startFreq = micros();
      } else if (micros() - startFreq > 8000) {
        startFreq = micros();
        count = 0;
        data = 0;
        bitC = 0;
      }
    }
    if(tmpZone == 0 && prev == 1) {
      count++;
      check2 = true;
    }
    if(tmpZone == 0 && prev == -1) {
      if (check2 == true && micros() - startFreq > 3500) {
        data <<= 2;
        data |= (count - 2) & B0011;
        bitC++;
        if (bitC == 4) {
          Serial.print((char)data);
          data = 0;
          bitC = 0;
        }
        check2 = false;
        count = 0;
      }
    }
    prev = tmpZone;
  }
}


int8_t zone(uint16_t val) {
  if (val <= 350) return -1;
  else if (val <= 600) return 0;
  else return 1;
}

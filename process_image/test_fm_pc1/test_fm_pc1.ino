#include <Arduino.h>
#include <Wire.h>
#include <TEA5767Radio.h>

#define NUM_SAMPLE 4
#define NUM_FREQ 4
#define FREQ_DIFF 250

int delay0;
float S[NUM_SAMPLE];
uint16_t S_DAC[NUM_SAMPLE];

uint16_t freq[NUM_FREQ];
uint16_t freqDelay[NUM_FREQ];

// defines for setting and clearing register bits for RX
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define r_slope 20
///////////////////////////////////////////////////

void setVoltage(uint16_t vol)
{
  Wire.beginTransmission(0x62);
  Wire.write((vol >> 8) & 0b00001111);
  Wire.write(vol & 0b11111111);
  Wire.endTransmission();
}

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  Wire.begin();
  Wire.setClock(400000UL);

  for (int i = 0; i < NUM_FREQ; i++)
  {
    freq[i] = (i+2) * FREQ_DIFF;
    freqDelay[i] = ((1000000 / freq[i]) / NUM_SAMPLE) - 110;
    Serial.print(freq[i]);
    Serial.print(" ");
    Serial.print(freqDelay[i]);
    Serial.print(" ");
    Serial.print(freq[i] / FREQ_DIFF);
    Serial.println();
  }

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
    Serial.print(S[i]);
    Serial.print(" ");
    Serial.print(S_DAC[i]);
    Serial.println();
  }
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  Serial.flush();
 
  setVoltage(2047.5);
}


int state = 1;
void tx(){
    if (Serial.available() > 0)
  {
    char in = Serial.read();
    if(in == 's'){
      state = 1;
    
    int input[4];
    input[0] = (in >> 0) & B0011;
    input[1] = (in >> 2) & B0011;
    input[2] = (in >> 4) & B0011;
    input[3] = (in >> 6) & B0011;
    
    for (int k = 3; k >= 0; k--)
    {
      for (int cycle = freq[input[k]] / FREQ_DIFF; cycle > 0; cycle--)
      {
        for (int sample = 0; sample < NUM_SAMPLE; sample++)
        {
          setVoltage(S_DAC[sample]);
          delayMicroseconds(freqDelay[input[k]]);
        }
      }
    }
    if (Serial.available() == 0){
      setVoltage(2047.5);
    }
  }
  }
}

int prev = 0;
int check2 = false;
int count = 0;

uint32_t startFreq = 0;

char data = 0;
uint8_t bitC = 0;

int8_t zone(uint16_t val) {
  if (val <= 350) return -1;
  else if (val <= 600) return 0;
  else return 1;
}

void rx(){
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
          Serial.print(char(data));
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



void loop()
{
  if(state == 1){
    tx();
  }else if(state == 2){
    rx();
  }
}

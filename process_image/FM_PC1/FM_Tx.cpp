#include "FM_Tx.h"

FM_Tx::FM_Tx()
{
  Wire.begin();
  Wire.setClock(800000UL);
  for (int i = 0; i < NUM_FREQ; i++)
  {
    freq[i] = (i + 2) * FREQ_DIFF;
    freqDelay[i] = ((1000000 / freq[i]) / NUM_SAMPLE) - 75;
  }

  for (int i = 0; i < NUM_SAMPLE; i++)
  {
    S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
    S_DAC[i] = S[i] * 2047.5 + 2047.5;
  }

  setVoltage(2047);
}

void FM_Tx::setVoltage(uint16_t vol)
{
  Wire.beginTransmission(0x62);
  Wire.write((vol >> 8) & 0b00001111);
  Wire.write(vol & 0b11111111);
  Wire.endTransmission();
}

void FM_Tx::sendFrame(char in[], int l) {
  sendFM(in, l + 3);
}

void FM_Tx::sendFM(char in[], int l) {
  if (l == 0) {
    l = strlen(in);
  }
  for (int i = 0; i < l; i++) {
    transmit(in[i]);
  }
  setVoltage(2047);
  delay(200);
}

void FM_Tx::sendFM(char in)
{
  transmit(in);
  setVoltage(2047);
  delay(200);
}

void FM_Tx::transmit(char in)
{
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
}

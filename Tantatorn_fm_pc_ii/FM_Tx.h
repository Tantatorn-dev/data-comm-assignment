#include <Arduino.h>
#include <Wire.h>

#define NUM_SAMPLE 8
#define NUM_FREQ 4
#define FREQ_DIFF 250

class FM_Tx
{
  public:
    FM_Tx();
    void sendFM(char data);
    void sendFM(char in[], int l = 0);
    void sendFrame(char in[], int l);

  private:
    int delay0;
    float S[NUM_SAMPLE];
    uint16_t S_DAC[NUM_SAMPLE];
    uint16_t freq[NUM_FREQ];
    uint16_t freqDelay[NUM_FREQ];

    void setVoltage(uint16_t vol);
    void transmit(char in);
};

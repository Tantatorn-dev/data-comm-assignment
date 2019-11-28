#include "FM_Rx.h"

FM_Rx::FM_Rx(float freq)
{
  // put your setup code here, to run once:
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  Wire.begin();
  radio.setFrequency(freq);

  this->CRC8 = FastCRC8();
}

/*
 * return
 * -6 not ack
 * -7 size not ack
 */

int FM_Rx::receiveAck(unsigned long timeout)
{
  uint8_t buff[1] = {0};
  int res = receiveFrame(buff, 2, 1, timeout);
  
  if (res == 1) {
    if (buff[0] == 'A') {
      return 1; 
    }
    return -6;
  } else if (res > 0) {
    return -7;
  }
  return res;
}

/*
 * return
 * -1 timeout
 * -2 not start byte
 * -3 size more than maxlen
 * -4 timeout during getdata
 * -5 crc error
 */


int FM_Rx::receiveFrame(uint8_t *buffer, uint8_t startByte, uint8_t maxlen, unsigned long timeout)
{
  int getStart = this->receiveFM(timeout);
  if (getStart == -1) return -1;
  else if (getStart == startByte)
  {
    int size = this->receiveFM(timeout);
    if (size > maxlen) {
      int temp = 0;
      size++;
      while(size-- && temp != -1) temp = this->receiveFM(timeout);
      return -3;
    }
    for (int i = 0; i < size + 1; i++)
    {
      int receive = this->receiveFM(timeout);
      if (receive == -1) return -4;
      buffer[i] = receive;
    }
    if (CRC8.smbus(buffer, size + 1) != 0)
    {
      return -5;
    }
    return size;
  }
  else return -2;
}

int FM_Rx::receiveFM(unsigned long timeout)
{
  unsigned long start = millis();

  int prev = 0;
  int check2 = false;
  int count = 0;
  uint32_t startFreq = 0;
  char data = 0;
  uint8_t bitC = 0;

  while (millis() - start < timeout || (startFreq != 0 && micros() - startFreq > 8000))
  {
    int8_t tmpZone = zone(analogRead(A2));
    if (tmpZone != prev)
    {
      if (tmpZone == 1 && prev != 1) // from 0 to 1
      {
        if (check2 == false)
        {
          startFreq = micros();
        }
        else if (micros() - startFreq > 8000)
        {
          startFreq = micros();
          count = 0;
          data = 0;
          bitC = 0;
        }
      }
      if (tmpZone == 0 && prev == 1) // from 1 to 0
      {
        count++;
        check2 = true;
      }
      if (tmpZone == 0 && prev == -1) // from -1 to 0
      {
        if (check2 == true && micros() - startFreq > 3500)
        {
          if (2 <= count && count <= 5)
          {
            data <<= 2;
            data |= (count - 2) & B0011;
            bitC++;
            if (bitC == 4)
            {
              // Serial.print((char)data);
              if (data != 0)
              {
                return data;
              }
              data = 0;
              bitC = 0;
            }
          }
          check2 = false;
          count = 0;
        }
      }
      prev = tmpZone;
    }
  }
  return -1;
}

int8_t FM_Rx::zone(uint16_t val)
{
  if (val <= 350)
    return -1;
  else if (val <= 600)
    return 0;
  else
    return 1;
}

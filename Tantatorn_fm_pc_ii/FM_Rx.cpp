#include "FM_Rx.h"

FM_Rx::FM_Rx()
{
    // put your setup code here, to run once:
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);

    radio.setFrequency(93.44);

}

char FM_Rx::receiveFM()
{   
    int result = 0;
    int8_t tmpZone = zone(analogRead(A2));
    if (tmpZone != prev)
    {
        if (tmpZone == 1 && prev != 1)
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
        if (tmpZone == 0 && prev == 1)
        {
            count++;
            check2 = true;
        }
        if (tmpZone == 0 && prev == -1)
        {
            if (check2 == true && micros() - startFreq > 3500)
            {
                data <<= 2;
                data |= (count - 2) & B0011;
                bitC++;
                if (bitC == 4)
                {
                   // Serial.print((char)data);
                    if (data != 0)
                    {
                        result = data;
                    }
                    data = 0;
                    bitC = 0;
                }
                check2 = false;
                count = 0;
            }
        }
        prev = tmpZone;
    }
    
    return result;
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
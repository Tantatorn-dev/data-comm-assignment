// ! Install Better comment
// ! Install Todo tree

// TODO: Test userInput
// TODO: Test init Freq
// TODO: Test init Sin Dac
// TODO: Test FM Begin
// TODO: Test Sent one Data
// TODO: Test sentFrame
// TODO: Test ReceiveOneData
// TODO: Test Is error

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include <Arduino.h>
#include <Wire.h>
#include <FastCRC.h>

FastCRC8 CRC8;

// ! Receive define
#define r_slope 20
char data = 0;
int check2 = false;
int count = 0;
int prev = 0;
uint32_t startFreq = 0;
uint8_t bitC = 0;
// ! Receive define

// ! FM define
#define NUM_SAMPLE 4
#define NUM_FREQ 4
#define FREQ_DIFF 250

int delay0;
float S[NUM_SAMPLE];
uint16_t S_DAC[NUM_SAMPLE];
uint16_t freq[NUM_FREQ];
uint16_t freqDelay[NUM_FREQ];
// ! FM define

// ? Flow Control
boolean startError = true;
boolean commandError = true;
// ? Flow Control

//? Data Frame
uint8_t startCRC[sizeof(startData) + 1];
uint8_t startFrame[sizeof(startCRC) + 2];
uint8_t errorData;
uint8_t checkError;
uint8_t firstData[6];

uint8_t ReceiveCount = 0;
uint8_t ReceiveError[sizeof(uint8_t) + 1];
// ? Data Frame

//? Default data
uint8_t startData = 's';
uint8_t top = '1';
uint8_t bottom = '2';
uint8_t left = '3';
uint8_t right = '4';
uint8_t upper = '5';
uint8_t lower = '6';
uint8_t nw = '7';     // -45
uint8_t middle = '8'; // 0
uint8_t ne = '9';     // 45
uint8_t flag = 2;
//? Default data

void sentFrame(uint8_t *data, uint8_t size);                     //!
void makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size);    //!
void makeCRC(uint8_t *buffer, uint8_t *base, uint8_t size_base); //!
int isError();                                                   //!
void ReceiveData();
void print(uint8_t *buffer, uint8_t size); //!
void printData();
void clearData();
void userStart(); //!
void userInput();
void makeCommand();
void printResult();
void analyzeData();
void sentOneData(char in);     //!
void setVoltage(uint16_t vol); //!
void wireBegin();              // !
void initFreq();               // !
void initSinDac();             // !
void FM_Begin();               // !
uint8_t ReceiveOneData();      //!
void Receive_Begin();          //!
int8_t zone(uint16_t val);     //!
void analyzeFirstData();
int CRCcheckError(uint8_t *buffer, uint8_t size);

// ! in comment => implemented
void setup()
{
    Serial.begin(9600);
    Serial.flush();
    //wireBegin();
    //Receive_Begin();
    //FM_Begin();
    //userStart();
    while (startError)
    {
        //makeCRC(startCRC, startData, sizeof(startData));
        //makeFrame(startFrame, startCRC, sizeof(startCRC));
        //sentFrame(startFrame, sizeof(sentFrame));
        //startError == isError();
        if (!startError)
        {
            //ReceiveData(firstData, sizeof(firstData));
            analyzeFirstData();
            printData();
            break;
        }
        clearData();
    }
    while (commandError)
    {
        userInput();
        makeCommand();
        //makeCRC();
        //makeFrame();
        //sentFrame();
        commandError = isError();
        if (!commandError)
        {
            ReceiveData();
            analyzeData();
            printResult();
            break;
        }
        clearData();
    }
}

void loop()
{
}

void wireBegin()
{
    Wire.begin();
    Wire.setClock(400000UL);
}

void setVoltage(uint16_t vol)
{
    Wire.beginTransmission(0x62);
    Wire.write((vol >> 8) & 0b00001111);
    Wire.write(vol & 0b11111111);
    Wire.endTransmission();
}

void initFreq()
{
    for (int i = 0; i < NUM_FREQ; i++)
    {
        freq[i] = (i + 2) * FREQ_DIFF;
        freqDelay[i] = ((1000000 / freq[i]) / NUM_SAMPLE) - 110;
        Serial.print(freq[i]);
        Serial.print(" ");
        Serial.print(freqDelay[i]);
        Serial.print(" ");
        Serial.print(freq[i] / FREQ_DIFF);
        Serial.println();
    }
}

void initSinDac()
{
    for (int i = 0; i < NUM_SAMPLE; i++)
    {
        S[i] = sin(DEG_TO_RAD * 360.0 / NUM_SAMPLE * i);
        S_DAC[i] = S[i] * 2047.5 + 2047.5;
        Serial.print(S[i]);
        Serial.print(" ");
        Serial.print(S_DAC[i]);
        Serial.println();
    }
}

void FM_Begin()
{
    initFreq();
    initSinDac();
    setVoltage(2047.5);
}

void sentFrame(uint8_t *data, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        sentOneData(char(data[i]));
    }
}

void Receive_Begin()
{
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
}
int8_t zone(uint16_t val)
{
    if (val <= 350)
        return -1;
    else if (val <= 600)
        return 0;
    else
        return 1;
}

uint8_t ReceiveOneData()
{
    uint8_t temp;
    while (true)
    {
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
                        Serial.print(data);
                        temp = data;
                        data = 0;
                        bitC = 0;
                        break;
                    }
                    check2 = false;
                    count = 0;
                }
            }
            prev = tmpZone;
        }
    }
    return temp;
}

void analyzeFirstData()
{
}

void ReceiveData(uint8_t *buffer, uint8_t size)
{
    uint8_t idx = 0;
    uint8_t dataWithCRC[size + 1];
    while (ReceiveCount != uint8_t(2))
    {
        uint8_t temp = ReceiveOneData();
        if (temp == flag)
        {
            count++;
        }
        else
        {
            dataWithCRC[idx] = temp;
            idx++;
        }
    }
    //Reset Counter
    ReceiveCount = 0;
    if (CRCcheckError(dataWithCRC, sizeof(dataWithCRC)))
    {
        //Sent something
    }
    else
    {
        //copy data with crc to buffer and cut CRCchecksum
        for (int i = 0; i < size; i++)
        {
            buffer[i] = dataWithCRC[i]
        }
    }
}

void printData()
{
}

int isError()
{

    ReceiveData(errorData, sizeof(errorData));
    return errorData;
}

void clearData()
{
}

void userInput()
{
}

void makeCommand()
{
}

void printResult()
{
}

void analyzeData()
{
}

int CRCcheckError(uint8_t *buffer, uint8_t size)
{
    uint8_t checkError = CRC8.smbus(buffer, size);
    if (checkError == uint8_t(0))
    {
        return 0;
    }
    return 1;
}

void makeCRC(uint8_t *buffer, uint8_t *base, uint8_t size_base)
{
    uint8_t remider_2 = CRC8.smbus(base, size_base);
    uint8_t idx = 0;
    for (; idx < size_base; idx++)
    {
        buffer[idx] = base[idx];
    }
    buffer[idx] = remider_2;
}

void makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    uint8_t idx = 0;
    buffer[idx] = 21;
    for (idx = 1; idx <= size; idx++)
    {
        buffer[idx] = data[idx - 1];
    }
    buffer[idx] = 21;
}

void print(uint8_t *buffer, uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void userStart()
{
    while (true)
    {
        Serial.println("Press s to start => ");
        while (!Serial.available())
        {
            char temp = Serial.read();
            if (temp == 's' || temp == 'S')
            {
                break;
            }
            Serial.println("Invalid Input");
        }
    }
}

void sentOneData(char in)
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
    setVoltage(2047.5);
}
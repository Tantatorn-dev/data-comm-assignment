#include <FastCRC.h>

FastCRC8 CRC8;

boolean startError = true;
boolean commandError = true;
uint8_t startFrame[3];

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

void sentFrame();
void makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size);
void makeCRC(uint8_t *buffer, uint8_t *base, uint8_t size_base);
int isError();
void ReceiveData();
void print(uint8_t *buffer, uint8_t size);
void printData();
void clearData();
void userStart();
void userInput();
void makeCommand();
void printResult();
void analyzeData();

void setup()
{
    Serial.begin(9600);
    userStart();
    while (startError)
    {
        //makeCRC();
        //makeFrame();
        sentFrame();
        startError == isError();
        if (!startError)
        {
            ReceiveData();
            printData();
            break;
        }
        clearData();
    }
    while (commandError)
    {
        userInput();
        makeCommand();
        makeCRC();
        makeFrame();
        sentFrame();
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

void makeCRC()
{
}

void makeFrame()
{
}
void sentFrame()
{
}

void ReceiveData()
{
}

void printData()
{
}

int isError()
{
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
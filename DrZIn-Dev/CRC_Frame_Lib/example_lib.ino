#include <CRC_FRAME.h>

CRC_FRAME crc;

uint8_t data[] = {'1', '7', '2', '8', '3', '9'};
uint8_t buffer[sizeof(data) + 1];
uint8_t frameBuffer[sizeof(buffer) + 2];
uint8_t flag = 2;
uint8_t frameDecrypt[sizeof(frameBuffer) - 2];

void printData(uint8_t *data, uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        Serial.print(String(data[i]) + "  ");
    }
    Serial.println();
}
void setup()
{
    Serial.begin(9600);
    Serial.println("CRC => " + String(crc.crc8(data, sizeof(data))));
    Serial.println("Get checksum => " + String(crc.getCheckSum()));
    crc.createCRC(buffer, data, sizeof(data));
    printData(buffer, sizeof(buffer));
    Serial.println();
    Serial.println("Check Error =>" + String(crc.checkError(buffer, sizeof(buffer))));
    crc.makeFrame(frameBuffer, buffer, sizeof(buffer), flag);
    printData(frameBuffer, sizeof(frameBuffer));
    crc.decryptFrame(frameDecrypt, frameBuffer, sizeof(frameBuffer));
    printData(frameDecrypt, sizeof(frameDecrypt));
}

void loop()
{
}
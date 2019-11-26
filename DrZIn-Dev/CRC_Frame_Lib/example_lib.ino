#include <CRC_FRAME.h>

CRC_FRAME crc;

uint8_t data[49];
uint8_t buffer[sizeof(data) + 1];
uint8_t frameBuffer[sizeof(buffer) + 2];
uint8_t flag = 2;
uint8_t frameDecrypt[sizeof(frameBuffer) - 2];
uint8_t crcDecrypt[sizeof(frameDecrypt) - 1];

uint8_t send_buffer[sizeof(data) + 3];
uint8_t receive_buffer[sizeof(data)];
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
    for (int i = 0; i < 49; i++)
    {
        data[i] = i;
    }
    printData(data, sizeof(data));

    Serial.println("CRC => " + String(crc.crc8(data, sizeof(data))));
    Serial.println("Get checksum => " + String(crc.getCheckSum()));

    crc.createCRC(buffer, data, sizeof(data));
    printData(buffer, sizeof(buffer));

    Serial.println("Check Error =>" + String(crc.checkError(buffer, sizeof(buffer))));

    crc.makeFrame(frameBuffer, buffer, sizeof(buffer), flag);
    printData(frameBuffer, sizeof(frameBuffer));

    crc.decryptFrame(frameDecrypt, frameBuffer, sizeof(frameBuffer));
    printData(frameDecrypt, sizeof(frameDecrypt));

    crc.decryCRC(crcDecrypt, frameDecrypt, sizeof(frameDecrypt));
    printData(crcDecrypt, sizeof(crcDecrypt));

    crc.send(send_buffer, data, sizeof(data), flag);
    printData(send_buffer, sizeof(send_buffer));

    crc.receive(receive_buffer, send_buffer, sizeof(send_buffer));
    printData(receive_buffer, sizeof(receive_buffer));
}

void loop()
{
}
#include <CRC_FRAME.h>

CRC_FRAME crc;

uint8_t data[5];
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
    Serial.begin(115200);
    for (int i = 0; i < 5; i++)
    {
        data[i] = char('a') + i;
    }
    Serial.print("Original Data => ");
    printData(data, sizeof(data));

    Serial.println("CRC => " + String(crc.crc8(data, sizeof(data))));
    Serial.println("Get checksum => " + String(crc.getCheckSum()));

    Serial.print("Data After create msg => ");
    crc.createCRC(buffer, data, sizeof(data));
    printData(buffer, sizeof(buffer));

    Serial.println("Check Error =>" + String(crc.checkError(buffer, sizeof(buffer))));

    Serial.print("Data After make Frame => ");
    crc.makeFrame(frameBuffer, buffer, sizeof(buffer), flag);
    printData(frameBuffer, sizeof(frameBuffer));

    Serial.print("Data After decrypt Frame => ");
    crc.decryptFrame(frameDecrypt, frameBuffer, sizeof(frameBuffer));
    printData(frameDecrypt, sizeof(frameDecrypt));

    Serial.print("Data After decrypt CRC => ");
    crc.decryCRC(crcDecrypt, frameDecrypt, sizeof(frameDecrypt));
    printData(crcDecrypt, sizeof(crcDecrypt));

    Serial.print("Data For send => ");
    crc.send(send_buffer, data, sizeof(data), flag);
    printData(send_buffer, sizeof(send_buffer));

    Serial.print("Data After Receive => ");
    crc.receive(receive_buffer, send_buffer, sizeof(send_buffer));
    printData(receive_buffer, sizeof(receive_buffer));
}

void loop()
{
}
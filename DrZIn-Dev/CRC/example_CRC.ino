#include <FastCRC.h>

FastCRC8 CRC8;

uint8_t buf[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

void create_MSG(uint8_t buffer, uint8_t base, uint8_t remider);
void create_MSG(uint8_t *buffer, uint8_t *base, uint8_t size_base, uint8_t remider)
{
    uint8_t idx = 0;
    for (; idx < size_base; idx++)
    {
        buffer[idx] = base[idx];
    }
    buffer[idx] = remider;
}

void setup()
{
    Serial.begin(9600);
    uint8_t remider = CRC8.smbus(buf, sizeof(buf));
    uint8_t buffer[sizeof(buf) + 1];
    create_MSG(buffer, buf, sizeof(buf), remider);
    uint8_t checkError = CRC8.smbus(buffer, sizeof(buffer));
    Serial.println("Is Error => " + String(checkError));
}

void loop()
{
}

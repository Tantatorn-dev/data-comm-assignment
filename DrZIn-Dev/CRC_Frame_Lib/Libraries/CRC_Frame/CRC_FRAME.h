#include <FastCRC.h>
class CRC_FRAME
{
private:
    FastCRC8 CRC8;
    uint8_t checkSum;

public:
    CRC_FRAME(/* args */);
    ~CRC_FRAME();

    void createCRC(uint8_t *buffer, uint8_t *data, uint8_t size);
    uint8_t crc8(uint8_t *data, uint8_t size);
    uint8_t checkError(uint8_t *data, uint8_t size);
    uint8_t getCheckSum();
    void makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag);
    void decryptFrame(uint8_t *buffer, uint8_t *data, uint8_t size);
};

CRC_FRAME::CRC_FRAME(/* args */)
{
    this->CRC8 = FastCRC8();
}

CRC_FRAME::~CRC_FRAME()
{
}

uint8_t CRC_FRAME::crc8(uint8_t *data, uint8_t size)
{
    this->checkSum = CRC8.smbus(data, size);
    return this->checkSum;
}

uint8_t CRC_FRAME::getCheckSum()
{
    return this->checkSum;
}

void CRC_FRAME::createCRC(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    this->checkSum = this->crc8(data, size);
    Serial.println("CRC => " + String(this->checkSum));
    uint8_t idx = 0;
    for (idx = 0; idx < size; idx++)
    {
        buffer[idx] = data[idx];
    }
    buffer[idx] = this->checkSum;
}

uint8_t CRC_FRAME::checkError(uint8_t *data, uint8_t size)
{
    uint8_t checkSum = this->crc8(data, size);
    if (checkSum == 0)
    {
        return 0;
    }
    return 1;
}

void CRC_FRAME::makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag)
{
    uint8_t idx = 0;
    buffer[idx] = flag;
    for (idx = 1; idx < size + 1; idx++)
    {
        buffer[idx] = data[idx - 1];
    }
    buffer[idx] = flag;
}

void CRC_FRAME::decryptFrame(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    uint8_t idx = 0;
    for (idx = 0; idx < size; idx++)
    {
        buffer[idx] = data[idx + 1];
    }
}
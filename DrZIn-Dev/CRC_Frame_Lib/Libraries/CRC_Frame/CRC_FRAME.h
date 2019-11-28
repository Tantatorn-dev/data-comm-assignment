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
    void decryCRC(uint8_t *buffer, uint8_t *data, uint8_t size);
    void send(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag);
    uint8_t receive(uint8_t *buffer, uint8_t *data, uint8_t size);
};

CRC_FRAME::CRC_FRAME(/* args */)
{
    this->CRC8 = FastCRC8();
}

CRC_FRAME::~CRC_FRAME()
{
}

uint8_t CRC_FRAME::receive(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    uint8_t bufferCRC[size - 2];
    this->decryptFrame(bufferCRC, data, size);
    uint8_t isError;
    isError = this->checkError(bufferCRC, sizeof(bufferCRC));
    if (isError == 0)
    {
        this->decryCRC(buffer, bufferCRC, sizeof(bufferCRC));
        return 0;
    }
    return 1;
}

void CRC_FRAME::send(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag)
{
    uint8_t bufferCRC[size + 1];
    this->createCRC(bufferCRC, data, size);
    this->makeFrame(buffer, bufferCRC, sizeof(bufferCRC), flag);
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
    uint8_t idx = 0;
    for (idx = 0; idx < size; idx++)
    {
        buffer[idx] = data[idx];
    }
    buffer[idx] = this->checkSum;
}

uint8_t CRC_FRAME::checkError(uint8_t *data, uint8_t size)
{
    return this->crc8(data, size);
}

void CRC_FRAME::makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag)
{
    uint8_t idx = 0, start_idx = 0, size_idx = 1;
    buffer[start_idx] = flag;
    buffer[size_idx] = size - 1;
    for (idx = 2; idx < size + 2; idx++)
    {
        buffer[idx] = data[idx - 2];
    }
}

void CRC_FRAME::decryptFrame(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    uint8_t idx = 0;
    for (idx = 0; idx < size; idx++)
    {
        buffer[idx] = data[idx + 2];
    }
}
void CRC_FRAME::decryCRC(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    for (int i = 0; i < size - 1; i++)
    {
        buffer[i] = data[i];
    }
}
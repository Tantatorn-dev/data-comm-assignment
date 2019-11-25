# CRC and Frame by DrZIn

## CRC

* use CRC-8 from FastCRC Lib

## Frame Structure

| Header | Body | checksum | tail |
| ------ | ---- | -------- | ---- |
| 2      |      | crc-8    | 2    |

## Data meaning

```Data => uint8_t```

| Data | Meaning |
| ---- | ------- |
| 2    | flag    |

## Prerequisites

```git clone https://github.com/Timefear007/data-comm-assignment.git```

install all folder => ***DrZIn-Dev\CRC_Frame_Lib\Libraries***

## How to use

```void createCRC(uint8_t *buffer, uint8_t *data, uint8_t size);```

| parameter | meaning          | size           |
| --------- | ---------------- | -------------- |
| buffer    | destination msg  | sizeof(data)+1 |
| data      | base msg         | sizeof(data)   |
| size      | size of **data** | sizeof(data)   |

--------------------------------
```uint8_t crc8(uint8_t *data, uint8_t size);```

**return** => uint8_t checksum of crc8

| parameter | meaning          | size         |
| --------- | ---------------- | ------------ |
| data      | base msg         | sizeof(data) |
| size      | size of **data** | sizeof(data) |

--------------------------------
```uint8_t checkError(uint8_t *data, uint8_t size);```

**return** => uint8_t 0 or 1

| parameter | meaning          | size         |
| --------- | ---------------- | ------------ |
| 0         | flase            |              |
| 1         | true             |              |
| data      | base msg         | sizeof(data) |
| size      | size of **data** | sizeof(data) |

--------------------------------
```void makeFrame(uint8_t *buffer, uint8_t *data, uint8_t size, uint8_t flag);```

| parameter | meaning          | size           |
| --------- | ---------------- | -------------- |
| buffer    | destination msg  | sizeof(data)+2 |
| data      | base msg         | sizeof(data)   |
| size      | size of **data** | sizeof(data)   |
| flag      | flag char ( 2 )  | 1     1        |

--------------------------------
```void decryptFrame(uint8_t *buffer, uint8_t *data, uint8_t size);```
| parameter | meaning          | size           |
| --------- | ---------------- | -------------- |
| buffer    | destination msg  | sizeof(data)-2 |
| data      | base msg         | sizeof(data)   |
| size      | size of **data** | sizeof(data)   |

--------------------------------

# Example

```
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
```
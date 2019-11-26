#include "FM_Tx.h"
<<<<<<< Updated upstream
#include <String.h>
=======
#include "FM_Rx.h"
#include <String.h> 
#include <CRC_FRAME.h>
>>>>>>> Stashed changes

FM_Rx *receiver;
FM_Tx *transmitter;

//------ Global Variable -----
enum STATES
{
  START,
  GET_3DATA,
  LAST_STATE
} state = START;

String lable[6] = {"Top", "Bottom", "Left", "Right", "Upper", "Lower"};
int8_t angle[3] = {-45, 0, 45};
char pos[3];
struct Cor16
{
  uint8_t x;
  uint8_t y;
  uint8_t color;
} points16[16];

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();
}

String getImageType(char key)
{
  int lableIndex = int(key) - 49;
  return lable[lableIndex];
}

void showAll()
{
  Serial.println("-- Data of 3 images received from PC2 --");
  for (int j = 0; j < 3; j++)
  {
    String imgType = getImageType(pos[j]);
    String imgReceive = String(j + 1) + ". " + imgType + " at " + angle[j] + " degree";
    Serial.println(imgReceive);
  }
  Serial.println();
}

void start()
{
  Serial.println("======= start program =======");
  while (state == START)
  {
    Serial.print("Press \'s\' to get 3 image types from PC2: ");
    while (!Serial.available())
      ;
    char in = Serial.read();
    Serial.println(in);
    if (in == 's')
    {
      Serial.println("Sending \'s\' to PC2...");
      transmitter->sendFM(in);
      state = GET_3DATA;
    }
    else
    {
      Serial.println("wrong input please try again!\n");
    }
  }
}

void get3ImageData()
{
  Serial.println("Getting 3 data from PC2...");
  uint32_t timeout = millis();
  int index = 0;
  while (index < 3)
  {
    int receive = receiver->receiveFM();
    if (receive != -1)
    {
      Serial.print(char(receive));
      Serial.print(" ");
      pos[index++] = char(receive);
      if (index == 3)
      {
        Serial.println();
        showAll();
        state = LAST_STATE;
        delay(300);
      }
    }
    if (millis() - timeout > 10000)
    {
      Serial.println("Time out of 10 seconds");
      state = START;
      break;
    }
  }
}

void showPoints16(char key)
{
  Serial.println("Display 16 points in struct of an " + getImageType(key) + " image");
  for (int i = 0; i < 16; i++)
  {
    String pointData = "x = " + String(points16[i].x) + ", y = " + String(points16[i].y) + ", color = " + String(points16[i].color);
    Serial.println(pointData);
  }
}

void lastState()
{
  while (state == LAST_STATE)
  {
    Serial.print("Choose Image ==> ");

    for (int i = 0; i < 3; i++)
    {
      char key = pos[i];
      String imgType = getImageType(key);
      String imgPrint = "[" + String(key) + "]->" + imgType + " ";
      Serial.print(imgPrint);
    }
    Serial.println(" [r]->reset program");
    Serial.print("Input : ");
    while (!Serial.available())
      ;
    char in = Serial.read();
    Serial.println(in);
    if (in == pos[0] or in == pos[1] or in == pos[2] or in == 'r')
    {
      String sendingStatus = "Sending " + String(in) + " to PC2...";
      Serial.print(sendingStatus);
      transmitter->sendFM(in);
      if (in == 'r')
      {
        Serial.println("reset program");
        Serial.println();
        state = START;
        break;
      }
      uint32_t timeout = millis();
      uint8_t rawIndex = 0;
      while (rawIndex < 48)
      {
        if (millis() - timeout > 10000)
        {
          Serial.println("Time out of 10 seconds");
          break;
        }
        int temp = receiver->receiveFM();
        if (temp != -1)
        {
          int pIndex = rawIndex / 3;
          if (rawIndex % 3 == 0)
            points16[pIndex].y = temp;
          else if (rawIndex % 3 == 1)
            points16[pIndex].x = temp;
          else
            points16[pIndex].color = temp;
          rawIndex++;
        }
      }
      if (rawIndex == 48)
      {
        showPoints16(in);
      }
      else
      {
        Serial.println("Data lost!! May be something wrong.");
        Serial.print("raw index = ");
        Serial.println(rawIndex);
      }
    }
    else
    {
      Serial.println("wrong input please try again!");
      Serial.println();
    }
  }
}

void loop()
{
  if (state == START)
  {
    start();
  }
  else if (state == GET_3DATA)
  {
    get3ImageData();
  }
  else if (state == LAST_STATE)
  {
    lastState();
  }
}

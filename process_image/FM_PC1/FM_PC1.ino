#include "FM_Rx.h"
#include "FM_Tx.h"
#include <String.h>

FM_Rx * receiver;
FM_Tx * transmitter;

//------ Global Variable -----
enum STATES
{
  START = 1, GET_3DATA, LAST_STATE
};

enum STATES state  = START;

String lable[6] = {"Top", "Bottom", "Left", "Right", "Upper", "Lower"};
int8_t angle[3] = { -45, 0, 45};
char pos[3];
uint8_t raw48[48];
struct Cor16
{
  uint8_t x;
  uint8_t y;
  uint8_t color;
} cor16[16];


void setup() {
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();
}

String getImageType(char key) {
  int lableIndex = int(key) - 49;
  return lable[lableIndex];
}

void showAll() {
  Serial.println("--Data received from PC2--");
  for (int j = 0; j < 3; j++) {
    String imgType = getImageType(pos[j]);
    Serial.println(String(j + 1) + ". " + imgType + " at " + String(angle[j]) + " degree");
  }
  Serial.println();
}

void start() {
  Serial.println("======= start program =======");
  while (state == START) {
    Serial.print("Press \'s\' to get 3 image types from PC2: ");
    while (!Serial.available());
    char in = Serial.read();
    Serial.println(in);
    if (in == 's') {
      Serial.println("Sending \'s\' to PC2...");
      transmitter->sendFM(in);
      state = GET_3DATA;
    }
    else {
      Serial.println("wrong input please try again!\n");
    }
  }

}


void get3DataFromPC2() {
  Serial.println("Getting 3 data from PC2...");
  uint32_t t = millis();
  int i = 0;
  while (i < 3) {
    int receive = receiver->receiveFM();
    if (receive != -1) {
      Serial.print(char(receive));
      Serial.print(" ");
      pos[i++] = char(receive);
      if (i == 3) {
        Serial.println();
        showAll();
        state = LAST_STATE;
        delay(300);
      }
    }
    if (millis() - t > 10000) {
      Serial.println("Time out of 10 seconds");
      state = START;
      break;
    }
  }
}

void showAllPoints(char key) {
  int indexShow = 0;
  Serial.println("Display 16 points of an " + getImageType(key) + " image");
  for (int j = 0; j < 16; j++) {
    for (int k = 0; k < 3; k++) {
      Serial.print(String(raw48[indexShow++]));
      Serial.print(" ");
    }
    Serial.println();
  }
}

void showStruct(char key) {
  Serial.println("Display 16 points in struct of an " + getImageType(key) + " image");
  for (int i = 0; i < 16; i++) {
    Serial.print("x = ");
    Serial.print(cor16[i].x);
    Serial.print(", y = ");
    Serial.print(cor16[i].y);
    Serial.print(", color = ");
    Serial.println(cor16[i].color);
  }

}

void lastState() {
  while (state == LAST_STATE) {
    Serial.print("Choose Image ==> ");

    for (int i = 0; i < 3; i++) {
      char key = pos[i];
      String imgType = getImageType(key);
      Serial.print("[");
      Serial.print(key);
      Serial.print("]->");
      Serial.print(imgType);
      Serial.print(" ");
    }
    Serial.println(" [r]->reset program");
    Serial.print("Input : ");
    while (!Serial.available());
    char in = Serial.read();
    Serial.println(in);
    if (in == pos[0] or in == pos[1] or in == pos[2] or in == 'r') {
      Serial.print("Sending ");
      Serial.print(in);
      Serial.println(" to PC2...");
      transmitter->sendFM(in);
      if (in == 'r') {
        Serial.println("reset program");
        Serial.println();
        state = START;
        break;
      }
      uint32_t ti = millis();
      int indexRaw = 0;
      while (indexRaw != 48) {
        if (millis() - ti > 15000) {
          Serial.println("Time out of 15 seconds");
          memset(raw48, 0, 48);
          break;
        }
        int temp = receiver->receiveFM();
        if (temp != -1) {
          raw48[indexRaw] = temp;
          int corIndex = indexRaw / 3;
          if (indexRaw % 3 == 0) cor16[corIndex].y = temp;
          else if (indexRaw % 3 == 1)cor16[corIndex].x = temp;
          else cor16[corIndex].color = temp;
          indexRaw++;
        }
      }
      if (indexRaw == 48) {
        showAllPoints(in);
        showStruct(in);
      } else {
        Serial.println("Data lost!! May be something wrong.");
      }
    }
    else {
      Serial.println("wrong input please try again!");
      Serial.println();
    }
  }
}


void loop() {
  if (state == START) {
    start();
  } else if (state == GET_3DATA) {
    get3DataFromPC2();
  } else if (state == LAST_STATE) {
    lastState();
  }
}

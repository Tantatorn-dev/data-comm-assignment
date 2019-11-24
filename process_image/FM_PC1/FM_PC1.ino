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
int angle[3] = { -45, 0, 45};
char pos[3];

void setup() {
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();
}

String getImageType(char key){
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
  while (i != 3) {
    char receive = receiver->receiveFM();
    if (receive != 0) {
      Serial.print(receive);
      Serial.print(" ");
      pos[i++] = receive;
      if (i == 3) {
        Serial.println();
        showAll();
        state = LAST_STATE;
        delay(300);
      }
    }
    if (millis() - t > 15000) {
      Serial.println("Time out of 15 seconds");
      state = START;
      break;
    }
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
      if (in == 'r'){
        Serial.println("reset program\n");
        state = START;
        break;
      }
      uint32_t ti = millis();
      while (true) {
        if(millis() - ti > 5000){
          Serial.println("Time out of 5 seconds");
          break;
        }
        char receive = receiver->receiveFM();
        if (in == pos[0] or in == pos[1] or in == pos[2]) {
          Serial.print("received ");
          Serial.print(receive);
          Serial.println(" from PC2");
          Serial.println("The image is "+getImageType(receive));
          Serial.println();
          break;
        }
      }
    }
    else {
      Serial.println("wrong input please try again!\n");
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

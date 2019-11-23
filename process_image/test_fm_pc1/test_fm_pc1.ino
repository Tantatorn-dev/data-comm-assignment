#include "FM_Rx.h"
#include "FM_Tx.h"
#include <String.h>

FM_Rx * receiver;
FM_Tx * transmitter;

void setup() {

  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();

}

int state = 1;
void testSend() {

  while (!Serial.available());
  char in = Serial.read();
  Serial.println(in);
  transmitter->sendFM(in);
  state = 2;
}

uint32_t t;
int i = 0;
char pos[3];
String lable[] = {"Top", "Bottom", "Left", "Right", "Upper", "Lower"};
int angle[] = { -45, 0, 45};

void showAll() {
  Serial.println("--Data received--");
  for (int j = 0; j < 3; j++) {
    int lableIndex = int(pos[j]) - 49;
    String imgType = lable[lableIndex];
    Serial.println(imgType + " at " + String(angle[j]));
  }
}


void loop() {
  if (state == 1) {
    testSend();
    Serial.println("Sending \'s\' to PC2");
    delay(300);
    t = millis();
  } else if (state == 2) {
    char receive = receiver->receiveFM();
    if (receive != 0) {
      Serial.println(receive);
      pos[i++] = receive;
      if (i == 3) {
        showAll();
        i = 0;
        state = 1;
        delay(300);
      }
    }

  }

}

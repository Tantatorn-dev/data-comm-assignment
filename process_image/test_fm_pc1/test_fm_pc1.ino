#include "FM_Rx.h"
#include "FM_Tx.h"
#include <String.h>

FM_Rx * receiver;
FM_Tx * transmitter;

//------ Global Variable -----
enum STATES
{
   START = 1, GET_3DATA
};

enum STATES state  = START;

String lable[6] = {"Top", "Bottom", "Left", "Right", "Upper", "Lower"};
int angle[3] = {-45, 0, 45};
char pos[3];

void setup() {
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();
}
void showAll() {
  Serial.println("--Data received--");
  for (int j = 0; j < 3; j++) {
    int lableIndex = int(pos[j]) - 49;
    String imgType = lable[lableIndex];
    Serial.println(String(j+1)+ ". " + imgType + " at " + String(angle[j]) + " degree");
  }
  Serial.println();
}

void start(){
  while(state == START){
    Serial.print("Press \'s\' for get 3 image types from PC2: ");
    while(!Serial.available());
    char in = Serial.read();
    Serial.println(in);
    if (in == 's'){
      Serial.println("Sending \'s\' to PC2...");
      transmitter->sendFM(in);
      state = GET_3DATA;
      delay(300);
    }
    else{
      Serial.println("wrong input please try again!\n");
    }
  }
  
}


void get3DataFromPC2(){
  Serial.println("Getting 3 data from PC2...");
  uint32_t t = millis();
  int i = 0;
  while(i != 3){
    char receive = receiver->receiveFM();
    if (receive != 0) {
      Serial.print(receive);
      Serial.print(" ");
      pos[i++] = receive;
      if (i == 3) {
        Serial.println();
        showAll();
        state = START;
        delay(300);
      }
    }
    if(millis() - t > 15000){
      Serial.println("Time out of 15 seconds");
      state = START;
      break;
    }
  } 
}


void loop() {
  if (state == START) {
    start();
  }else if(state == GET_3DATA){
    get3DataFromPC2();
  }

}

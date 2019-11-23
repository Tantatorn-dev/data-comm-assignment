#include "pc_ii_state.h"
#include "FM_Rx.h"

#include <Servo.h>
Servo servoPan;
Servo servoTilt;

FM_Rx * receiver;

void setup() {

  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx();

  Wire.begin();
  
  initServo();
}

// Global variables

char data_pc2 = 0;
char  pos[3];
char  angle[3];
bool isRotationStart = false;

void loop() {

  if (state == AWAITING_PC1) {
    // awaiting commands from PC1
    if(receiver->receiveFM() == 's') 
    state = AWAITING_PC2;
  }
  else if (state == AWAITING_PC2) {
    // rotate camera and receive values
    receiveSerialPC2();
  }
  else if (state == SENDING_PC1) {
    
  }

}


// user defined functions

void initServo() {
  servoPan.attach(7);
  servoTilt.attach(8);

  servoTilt.write(50);
  servoPan.write(70);
}


void receiveSerialPC2() {
  char data_pc2 = 0;
  bool isAtLeft=false,isAtMid=false,isAtRight=false;
  isAtLeft = true;
  rotate_camera('l');
  angle[0] = 7;
  delay(400);
  while(isAtLeft){
    Serial.println("a");
    while(!Serial.available());
    data_pc2 = Serial.read();
    if(data_pc2 != 0) {
      pos[0] = data_pc2;
      rotate_camera('m');
      isAtLeft = false;
      isAtMid = true;
      data_pc2 = 0;
    }
  }
  delay(400);
  while(isAtMid){
    Serial.println("b");
    while(!Serial.available());
    data_pc2 = Serial.read();
    if(data_pc2 != 0) {
      pos[1] = data_pc2;
      rotate_camera('r');
      isAtMid = false;
      isAtRight = true;
      data_pc2 = 0;
    }
  }
  delay(400);
  while(isAtRight){
    Serial.println("c");
    while(!Serial.available());
    data_pc2 = Serial.read();
    if(data_pc2 != 0) {
      pos[2] = data_pc2;
      isAtRight = false;
      data_pc2 = 0;
    }
  }
  state = SENDING_PC1;
}

void rotate_camera(char direction_camera) {
  switch (direction_camera) {
    case 'l':
      servoTilt.write(66);
      servoPan.write(140);
      break;
    case 'm':
      servoTilt.write(66);
      servoPan.write(98);
      break;
    case 'r':
      servoTilt.write(71);
      servoPan.write(43);
      break;
  }
}

void sendFM(){
  
}

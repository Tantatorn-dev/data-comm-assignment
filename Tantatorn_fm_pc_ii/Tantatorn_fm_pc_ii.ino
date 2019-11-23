#include "pc_ii_state.h"
#include "FM_Rx.h"
#include "FM_Tx.h"

#include <Servo.h>
Servo servoPan;
Servo servoTilt;

FM_Rx *receiver;
FM_Tx *transmitter;

void setup()
{

  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(106.54);
  transmitter = new FM_Tx();

  initServo();
}

// Global variables
char pos[4] = {0, 0, 0, 0};
char lastServoPosition = 'a';

void loop()
{

  if (state == AWAITING_PC1)
  {
    // awaiting commands from PC1
    if (receiver->receiveFM() == 's')
      state = AWAITING_PC2;
  }
  else if (state == AWAITING_PC2)
  {
    // rotate camera and receive values
    receiveSerialPC2();
  }
  else if (state == SENDING_PC1)
  {
    transmitter->sendFM(pos);
    state = LAST_STATE;
  }
  else if (state == LAST_STATE)
  {
    char dataIn = receiver->receiveFM();
    for (int i = 0; i < 3; i++)
    {
      if(dataIn == pos[i]){
        char dataIn2;
        if     (i==0) dataIn2 = captureAt('l');
        else if(i==1) dataIn2 = captureAt('m');
        else if(i==2) dataIn2 = captureAt('r');
        
        transmitter->sendFM(dataIn2);
      }
    }
    
    // reset system
    if(dataIn=='r'){
      state = AWAITING_PC1;
    }
  }
}

// user defined functions

void initServo()
{
  servoPan.attach(7);
  servoTilt.attach(8);

  rotate_camera('r');
}

void receiveSerialPC2()
{
  pos[0] = captureAt('l');
  pos[1] = captureAt('m');
  pos[2] = captureAt('r');
  
  state = SENDING_PC1;
}

char captureAt(char direction_camera)
{
  rotate_camera(direction_camera);
  return capture();
}

char capture()
{
  while (true)
  {
    Serial.println('c');
    while (!Serial.available())
      ;
    char data_pc2 = Serial.read();
    if (data_pc2 != 0)
    {
      return data_pc2;
    }
  }
}

void rotate_camera(char direction_camera)
{
  switch (direction_camera)
  {
  case 'l':
    servoTilt.write(69);
    servoPan.write(141);
    break;
  case 'm':
    if (lastServoPosition == 'l') {
      servoTilt.write(70);
      servoPan.write(94);
    } else if (lastServoPosition == 'r') {
      servoTilt.write(70);
      servoPan.write(105);
    }
    break;
  case 'r':
    servoTilt.write(71);
    servoPan.write(47);
    break;
  }
  lastServoPosition = direction_camera;
  delay(200);
}

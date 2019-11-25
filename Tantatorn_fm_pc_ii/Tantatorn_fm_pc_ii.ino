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
    state = SENDING_PC1;
  }
  else if (state == SENDING_PC1)
  {
    transmitter->sendFM(pos);
    state = LAST_STATE;
  }
  else if (state == LAST_STATE)
  {
    char dataIn = receiver->receiveFM();
    if (dataIn != 0)
    {
      for (int i = 0; i < 3; i++)
      {
        if (dataIn == pos[i]) {
          char dataIn2[48];
          if     (i == 0) captureColorAt('l', dataIn2);
          else if (i == 1) captureColorAt('m', dataIn2);
          else if (i == 2) captureColorAt('r', dataIn2);

          transmitter->sendFM(dataIn2, 48);
          Serial.println("D send Fin");
        }
      }

      // reset system
      if (dataIn == 'r')
      {
        state = AWAITING_PC1;
      }
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
}

char captureAt(char direction_camera)
{
  char a = '0';
  while(a == '0') {
    rotate_camera(direction_camera);
    
    Serial.println('c');
    while (!Serial.available());
    a =  Serial.read();
  }
  return a;
}

void captureColorAt(char direction_camera, uint8_t out[])
{
  out[0] = 0;
  while(out[0] == 0) {
    rotate_camera(direction_camera);
    
    Serial.println('x');
    while (!Serial.available());
    Serial.print("D read ");
    out[0] = Serial.read();
    Serial.print((int)out[0]);
    Serial.print(" ");
    if (out[0] == 0) {
      Serial.println("D con");
      continue;
    }
    for (int i = 1; i < 48; i++) {
      out[i] = Serial.read();
      Serial.print((int)out[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
}

void rotate_camera(char direction_camera)
{
  if (direction_camera == lastServoPosition) {
    switch (direction_camera)
    {
      case 'l': rotate_camera('m'); break;
      case 'm': rotate_camera('r'); break;
      case 'r': rotate_camera('m'); break;
    }
  }
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

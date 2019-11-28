#include "pc_ii_state.h"
#include "FM_Rx.h"
#include "FM_Tx.h"

#include <Servo.h>
#include <CRC_FRAME.h>
Servo servoPan;
Servo servoTilt;

FM_Rx *receiver;
FM_Tx *transmitter;

CRC_FRAME crc;

uint8_t buff[10];

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(107.84);
  transmitter = new FM_Tx();

  initServo();
}

// Global variables
char pos[4] = {0, 0, 0, 0};
char lastServoPosition = 'a';
bool cameraError = false;

int sendAndWaitAck(uint8_t *data, uint8_t size, unsigned long timeout)
{
  uint8_t dataOut2[size + 3];
  memset(dataOut2, 0, size + 3);
  
  crc.send(dataOut2, data, size, 2);

  int temp = -10;
  while(temp != 1){
    transmitter->sendFrame(dataOut2, size);
    Serial.println("D FM transmit");
  
    temp = receiver->receiveAck(timeout);
  }
  return temp;
}

int receiveAndSendAck(uint8_t *buffer, uint8_t maxlen, unsigned long timeout)
{
  int size = receiver->receiveFrame(buffer, 2, maxlen);
  if (size > 0) {
    uint8_t data = 'A';
    uint8_t dataOut2[5];
    memset(dataOut2, 0, 4);
    delay(250);
    crc.send(dataOut2, &data, 1, 2);
    transmitter->sendFrame(dataOut2, 1);
  }
  return size;
}

void loop()
{
  if (state == AWAITING_PC1)
  {
    // awaiting commands from PC1
    int size = receiveAndSendAck(buff, 10, 2000);
    Serial.print("D A get");
    Serial.println(size);
    if (size == 1) {
      if (buff[0] == 's') state = AWAITING_PC2;
    }
  }
  else if (state == AWAITING_PC2)
  {
    // rotate camera and receive values
    receiveSerialPC2();
    state = SENDING_PC1;
    Serial.println("D Receive PC");
  }
  else if (state == SENDING_PC1)
  {
    sendAndWaitAck(pos, 3, 2000);
    
    state = LAST_STATE;
  }
  else if (state == LAST_STATE)
  {
    int size = receiveAndSendAck(buff, 10, 2000);
    Serial.print("D D get");
    Serial.println(size);
    if (size > 0) {
      if (buff[0] == 'r')
      {
        // reset system
        state = AWAITING_PC1;
        rotate_camera('r');
        Serial.println("D reset");
      }
      else if ('1' <= buff[0] && buff[0] <= '6')
      {
        uint8_t dataIn2[48];
        memset(dataIn2, 0, 48);

        if      (buff[0] == pos[0]) captureColorAt('l', dataIn2);
        else if (buff[0] == pos[1]) captureColorAt('m', dataIn2);
        else if (buff[0] == pos[2]) captureColorAt('r', dataIn2);

        
        sendAndWaitAck(dataIn2, 48, 2000);
      }
      else if (buff[0] == 's')
      {
        state = AWAITING_PC2;
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
  while (a == '0')
  {
    rotate_camera(direction_camera);
    Serial.println('c');
    while (!Serial.available());
    a = Serial.read();
    if (a == '0')
    {
      cameraError = true;
    }
  }
  return a;
}

void captureColorAt(char direction_camera, uint8_t out[])
{
  out[0] = 0;
  while (out[0] == 0)
  {
    rotate_camera(direction_camera);
    Serial.println('x');
    while (!Serial.available());
    out[0] = Serial.read();
    if (out[0] == 0)
    {
      cameraError = true;
      continue;
    }
    for (int i = 1; i < 48; i++)
    {
      while (!Serial.available());
      out[i] = Serial.read();
    }
  }
}

void rotate_camera(char direction_camera)
{
  Serial.print("D rc ");
  Serial.println(direction_camera);

  if (cameraError) {
    cameraError = false;
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
        delay(100);
      }
      break;
    case 'r':
      servoTilt.write(71);
      servoPan.write(47);
      break;
  }
  lastServoPosition = direction_camera;
  delay(250);
}

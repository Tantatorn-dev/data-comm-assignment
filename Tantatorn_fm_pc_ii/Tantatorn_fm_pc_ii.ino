// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include <Wire.h>

#include <TEA5767Radio.h>
TEA5767Radio radio = TEA5767Radio();

#include <Servo.h>
Servo servoPan;
Servo servoTilt;

#define r_slope 20
void setup() {
  // put your setup code here, to run once:
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  Serial.begin(115200);
  Serial.flush();

  Wire.begin();
  radio.setFrequency(93.44);

  initServo();
}

// Defined enum
enum pc2_fm_state {
  AWAITING_PC1,
  AWAITING_PC2,
  SENDING_PC1
};

// Global variables

int prev = 0;
int check2 = false;
int count = 0;

uint32_t startFreq = 0;

char data_pc1 = 0;
uint8_t bitC = 0;

pc2_fm_state state = AWAITING_PC1;

char data_pc2 = 0;
char  pos[3];
char  angle[3];
bool isRotationStart = false;

void loop() {

  if (state == AWAITING_PC1) {
    // awaiting commands from PC1
    receiveFM();
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

void receiveFM() {
  int8_t tmpZone = zone(analogRead(A2));
  if (tmpZone != prev) {
    if (tmpZone == 1 && prev != 1) {
      if (check2 == false) {
        startFreq = micros();
      } else if (micros() - startFreq > 8000) {
        startFreq = micros();
        count = 0;
        data_pc1 = 0;
        bitC = 0;
      }
    }
    if (tmpZone == 0 && prev == 1) {
      count++;
      check2 = true;
    }
    if (tmpZone == 0 && prev == -1) {
      if (check2 == true && micros() - startFreq > 3500) {
        data_pc1 <<= 2;
        data_pc1 |= (count - 2) & B0011;
        bitC++;
        if (bitC == 4) {
          // Serial.print((char)data_pc1);
          if (data_pc1 == 's') {   
            state = AWAITING_PC2;
          }
          data_pc1 = 0;
          bitC = 0;
        }
        check2 = false;
        count = 0;
      }
    }
    prev = tmpZone;
  }
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

int8_t zone(uint16_t val) {
  if (val <= 350) return -1;
  else if (val <= 600) return 0;
  else return 1;
}
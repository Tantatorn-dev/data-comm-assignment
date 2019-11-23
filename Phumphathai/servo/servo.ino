#include <Servo.h>
Servo servoPan;
Servo servoTilt;
void setup()
{
  Serial.begin(115200);
  servoPan.attach(7);
  servoTilt.attach(8);
  
  servoTilt.write(90);
  servoPan.write(47);
}

char last = 'a';
void loop()
{
  if (Serial.available()) {
    char in = Serial.read();
    if (in == 'a') {
      servoTilt.write(71);
      servoPan.write(47);
      last = 'a';
    } else if (in == 'b') {
      if (last == 'c') {
        servoTilt.write(70);
        servoPan.write(94);
      } else if (last == 'a') {
        servoTilt.write(70);
        servoPan.write(105);
      }
    } else if (in == 'c') {
      servoTilt.write(69);
      servoPan.write(141);
      last = 'c';
    } else if (in == 't') {
      int num = Serial.parseInt();
      servoTilt.write(num);
    } else if (in == 'p') {
      int num = Serial.parseInt();
      servoPan.write(num);
    }
  }
}

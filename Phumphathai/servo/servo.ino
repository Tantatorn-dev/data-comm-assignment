#include <Servo.h>
Servo servoPan;
Servo servoTilt;
void setup()
{
  Serial.begin(115200);
  servoPan.attach(7);
  servoTilt.attach(8);
  
  servoTilt.write(50);
  servoPan.write(70);
}
void loop()
{
  if (Serial.available()) {
    char in = Serial.read();
    if (in == 'a') {
      servoTilt.write(71);
      servoPan.write(43);
    } else if (in == 'b') {
      servoTilt.write(66);
      servoPan.write(98);
    } else if (in == 'c') {
      servoTilt.write(66);
      servoPan.write(140);
    }
  }
}

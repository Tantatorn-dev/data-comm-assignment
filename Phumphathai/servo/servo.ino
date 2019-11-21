#include <Servo.h>
Servo myservo; //ประกาศตัวแปรแทน Servoกำหนดข
Servo myservo2;
void setup()
{
myservo.attach(7); // กำหนดขา 9 ควบคุม Servo
myservo2.attach(8);
myservo2.write(70);
myservo.write(98);
}
void loop()
{
  myservo2.write(71);
  myservo.write(43); // สั่งให้ Servo หมุนไปองศาที่ 0
  delay(20000); // หน่วงเวลา 1000ms
  myservo2.write(66);
  myservo.write(98); // สั่งให้ Servo หมุนไปองศาที่ 90
  delay(20000); // หน่วงเวลา 1000ms
  myservo2.write(66);
  myservo.write(140); // สั่งให้ Servo หมุนไปองศาที่ 180
  delay(20000); // หน่วงเวลา 1000ms
}

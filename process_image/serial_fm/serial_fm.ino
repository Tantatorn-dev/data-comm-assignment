
int led = 7;
void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
}

void receiveData() {
  if (Serial.available() > 0) {
    char in = Serial.read();
    digitalWrite(led, HIGH);
    delay(500);
    if (in == '1') {
      Serial.println("top");
    } else if (in == '2') {
      Serial.println("bottom");
    } else if (in == '3') {
      Serial.println("left");
    } else if (in == '4') {
      Serial.println("right");
    } else if (in == '5') {
      Serial.println("upper");
    } else if (in == '6') {
      Serial.println("lower");
    }

  }
}

void loop() {
  receiveData();
}

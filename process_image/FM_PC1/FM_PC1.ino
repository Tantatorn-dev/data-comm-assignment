
#include "FM_Tx.h"
#include "FM_Rx.h"

#include <CRC_FRAME.h>

FM_Rx *receiver;
FM_Tx *transmitter;

//------ Global Variable ------//
enum STATES {
  START,
  GET_3DATA,
  LAST_STATE
} state = START;

String lable[6] = {"Top", "Bottom", "Left", "Right", "Upper", "Lower"};
int8_t angle[3] = { -45, 0, 45};
char pos[3];
struct Cor16 {
  uint8_t x;
  uint8_t y;
  uint8_t color;
} points16[16];
char buff[100];

//--- CRC ---//
CRC_FRAME crc;
uint8_t rawData[51];
uint8_t unpackedData[48];

void setup() {
  Serial.begin(115200);
  Serial.flush();

  receiver = new FM_Rx(97);
  transmitter = new FM_Tx();
}


int sendAndWaitAck(uint8_t *data, uint8_t size, unsigned long timeout)
{
  uint8_t dataOut2[size + 3];
  memset(dataOut2, 0, size + 3);
  
  crc.send(dataOut2, data, size, 2);
  transmitter->sendFrame(dataOut2, size);

  int temp = -10;
  while(temp != 1){
    temp = receiver->receiveAck(timeout);
    if(temp!=1){
      Serial.println("timeout or error\nretransmitt...");
      transmitter->sendFrame(dataOut2, size);
    } else if (temp == 1) {
      Serial.println("Receive ACK");
    }
  }
  return temp;
}

int receiveAndSendAck(uint8_t *buffer, uint8_t maxlen)
{
  int size = receiver->receiveFrame(buffer, 2, maxlen);
  if (size > 0) {
    uint8_t data[] = {'A'};
    uint8_t dataOut2[4];
    memset(dataOut2, 0, 4);
    delay(250);
    crc.send(dataOut2, data, 1, 2);
    transmitter->sendFrame(dataOut2, 1);
  }
  return size;
}

String get_image_type(char key) {
  int lableIndex = int(key) - 49;
  return lable[lableIndex];
}

void show_img_types() {
  Serial.println("-- Data of 3 images received from PC2 --");
  for (int j = 0; j < 3; j++) {
    String imgType = get_image_type(pos[j]);
    String imgReceive = String(j + 1) + ". " + imgType + " at " + angle[j] + " degree";
    Serial.println(imgReceive);
  }
  Serial.println();
}

void get_image_data3() {
  char startCMD = 's';
  Serial.println("Sending \'s\' to PC2...");
  int temp = sendAndWaitAck(&startCMD, 1, 2000);
  delay(300);
  int size = -10;
  while (size <= 0) {
    size = receiveAndSendAck(buff, 3);
  }
  for(int i =0; i<3;i++){
    Serial.println("buff = " + String(buff[i]));
  }
  
  if (size == 3) {
    for (int i = 0; i < 3; i++) {
      pos[i] = buff[i];
    }
    show_img_types();
    state = LAST_STATE;
    delay(300);
  } else {
    Serial.println("Error data lost");
  }
}

void show_pics() {
  Serial.println("--- image 4x4 from 16 points ---");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      uint8_t index = (i * 4) + j;
      uint8_t color = points16[index].color;
      char buff[10];
      sprintf(buff, "%-5d ", color);
      Serial.print(buff);
    }
    Serial.println();
  }
}

void show_points16(char key) {
  Serial.println("Display 16 points in struct of an " + get_image_type(key) + " image");
  Serial.println("Description: color(255=white, 0=black)");
  for (int i = 0; i < 16; i++) {
    String pointData = "x = " + String(points16[i].x) + ", y = " + String(points16[i].y) + ", color = " + String(points16[i].color);
    Serial.println(pointData);
  }
}

void show_raw_data() {
  Serial.println("--- Raw data ---");
  Serial.println("head = " + String(rawData[0]));
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 3; j++) {
      uint8_t index = i * 3 + j + 1;
      Serial.print(rawData[index]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("CRC remainder = " + String(rawData[49]));
  Serial.println("tail = " + String(rawData[50]));
}

void show_data() {
  Serial.println("--- Unpacked data ---");
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 3; j++) {
      uint8_t index = i * 3 + j;
      Serial.print(unpackedData[index]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void data_to_struct() {
  for (int i = 0; i < 16; i++) {
    int index = i * 3;
    points16[i].y = unpackedData[index] ;
    points16[i].x = unpackedData[index + 1];
    points16[i].color = unpackedData[index + 2];
  }
}

void send_ack(char in) {
  String sendingStatus = "Sending " + String(in) + " to PC2 again...";
  Serial.println(sendingStatus);
  crc.send(buff, &in, 1, 2);
  transmitter->sendFrame(buff, 1);
}

//=============== START =================//
void start() {
  Serial.println("======= start program =======");
  while (state == START) {
    Serial.print("Press \'s\' to get 3 image types from PC2: ");
    while (!Serial.available()) {
    }
    char in = Serial.read();
    Serial.println(in);
    if (in == 's') {
      state = GET_3DATA;
    } else {
      Serial.println("wrong input please try again!\n");
    }
  }
}

void get_points16(char in) {
  uint32_t timeout = millis();
   int size = receiveAndSendAck(buff, 48);
   while(true){
    if(size == 48){
      for(int i=0; i<size;i++){
        unpackedData[i] = buff[i]; 
      }
      //show_data();
      data_to_struct();
      show_points16(in);
      show_pics();
      break;
    }else{
      size = receiveAndSendAck(buff, 48);
    }
   }
}

void last_state() {
  while (state == LAST_STATE) {
    Serial.print("Choose Image ==> ");

    for (int i = 0; i < 3; i++) {
      char key = pos[i];
      String imgType = get_image_type(key);
      String imgPrint = "[" + String(key) + "]->" + imgType + " ";
      Serial.print(imgPrint);
    }
    Serial.println(" [r]->reset program");
    Serial.print("Input : ");
    while (!Serial.available()) {
      receiveAndSendAck(buff, 48);
    }
    char in = Serial.read();
    Serial.println(in);
    if (in == pos[0] or in == pos[1] or in == pos[2] or in == 'r') {
      String sendingStatus = "Sending " + String(in) + " to PC2...";
      Serial.println(sendingStatus);
      sendAndWaitAck(&in, 1, 2000);
      if (in == 'r') {
        Serial.println("reset program");
        Serial.println();
        state = START;
      } else {
        get_points16(in);
      }
    } else if (in == 's') {
      Serial.println("reset program");
      Serial.println();
      state = START;
    } else {
      Serial.println("wrong input please try again!");
      Serial.println();
    }
  }
}

void loop() {
  if (state == START) {
    start();
  } else if (state == GET_3DATA) {
    get_image_data3();
  } else if (state == LAST_STATE) {
    last_state();
  }
}

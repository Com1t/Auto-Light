#include <SoftwareSerial.h>
#define RXD 6
#define TXD 5
#define EN 12
#define IN 4

SoftwareSerial RC(TXD,RXD);   //RX TX

void setup() {
  RC.begin(9600);
  pinMode(IN,INPUT);
  pinMode(EN,OUTPUT);
}

void loop() {  
  digitalWrite(EN,HIGH);
  bool state = digitalRead(4);
  if(!state){
    digitalWrite(EN,LOW);
    delay(1);
    RC.print("OPEN");
    delay(200);
  }
}

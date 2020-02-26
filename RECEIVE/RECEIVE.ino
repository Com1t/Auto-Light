#include <SoftwareSerial.h>
#define RXD 6
#define TXD 5
const uint8_t code[4] = {'O','P','E','N'};
SoftwareSerial RC( TXD, RXD); // RX, TX

void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  RC.begin(9600);
}

void loop() {
  uint8_t i = 0;
  while(RC.available()){
    if( RC.read() != code[i] ){
      break;
    }
    delay(1);
    i ++;
    if( i == 4 ){
      lightOn(13);
    }
  }
  lightOff(13);
}
void lightOn( uint8_t pin ){
    digitalWrite( pin, HIGH);
    delay(1000);
}
void lightOff( uint8_t pin ){
    digitalWrite( pin, LOW);
}


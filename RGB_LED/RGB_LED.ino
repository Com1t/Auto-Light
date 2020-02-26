#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
// Interrupt pin
#define readPin 2
// RX TX pin of RC device
#define RXD 3
#define TXD 4
// led pins
#define R 9
#define G 6
#define B 5
// EEPROM addresses
#define counterAddr 0x000
#define Raddr 0x001
#define Gaddr 0x002
#define Baddr 0x003
// DS3231 and time configuration
#define I2C_ADDRESS 0x68
#define startTime 1630
#define stopTime 2330
// MAX44009 I2C address is 0x4A(74)
#define lightAddr 0x4A

// open light code
const uint8_t code[4] = {'O','P','E','N'};
// open light gap time
const uint32_t lightGap = 10000;
uint32_t lastTime = 0;
uint32_t thisTime = millis();
// debouncing parameters
const uint8_t gap = 200;
volatile uint32_t postime = 0;
volatile uint32_t nowtime = millis();
// R, G, B luminance
volatile uint8_t Rlumi = 0;
volatile uint8_t Glumi = 0;
volatile uint8_t Blumi = 0;
// now phase
volatile uint8_t counter = 0;

SoftwareSerial RC( TXD, RXD); // RX, TX

uint8_t bcdTodec(uint8_t val);
uint16_t getTime();
float getLumi();
void stateChange();
void Change();
void smoothChange();
void(* resetFunc) (void) = 0;

void setup() {
  RC.begin(9600);
  Wire.begin();
  pinMode( 13, OUTPUT);
  pinMode( R, OUTPUT);
  pinMode( G, OUTPUT);
  pinMode( B, OUTPUT);
  pinMode( readPin, INPUT);
  Wire.beginTransmission(lightAddr);
  // Select configuration register
  Wire.write(0x02);
  // Continuous mode, Integration time = 800 ms
  Wire.write(0x80);
  // Stop I2C transmission
  Wire.endTransmission();
  counter = EEPROM.read(counterAddr);
  attachInterrupt(digitalPinToInterrupt(readPin), stateChange, RISING);
}


void loop() {
  uint8_t j = 0;
  char readCode[4] = {};
  while(RC.available()){
    readCode[j] = RC.read();
    delay(1);
    j++;
    if ( j == 4 ){
      j = 0;
      for( int i = 0; i < 4; i++ ){
        if( readCode[i] == code[i] ){
          j++;
        }
        if( j == 4 ){
          thisTime = millis();
          if( (thisTime - lastTime) >= lightGap ){
            lastTime = millis();
            uint16_t curTime = getTime();
            float lumi = getLumi();
            if( (curTime > startTime) && (curTime < stopTime) && (counter == 0) && (lumi <= 5) ){
              smoothChange();
              break;
            }
          }
        }
      }
    }
  }
  switch(counter){
    case 0:
      digitalWrite( R, LOW);
      digitalWrite( G, LOW);
      digitalWrite( B, LOW);
      break;
    case 1:
      change();
      break;
    case 2:
      smoothChange();
      break;
    case 3:
      analogWrite( R, EEPROM.read(Raddr));
      analogWrite( G, EEPROM.read(Gaddr));
      analogWrite( B, EEPROM.read(Baddr));
      break;
  }
}

void stateChange(){
    nowtime = millis();
    if( (nowtime - postime) >= gap ){
      counter ++;
      if( counter == 4){
        counter = 0;
      }
      EEPROM.write( Raddr,Rlumi);
      EEPROM.write( Gaddr,Glumi);
      EEPROM.write( Baddr,Blumi);
      EEPROM.write( counterAddr, counter);
      postime = millis();
      resetFunc();
    }
    else{
      return 0;
    }
}

void change() {
  // White
    digitalWrite( R, HIGH);
    digitalWrite( G, HIGH);
    digitalWrite( B, HIGH);
    delay(2500);
  // Yellow
    digitalWrite( B, LOW);
    delay(2500);
  // Green
    digitalWrite( R, LOW);
    delay(2500);
  // Light Blue
    digitalWrite( B, HIGH);
    delay(2500);
  // Deep Blue
    digitalWrite( G, LOW);
    delay(2500);
  // Purple
    digitalWrite( R, HIGH);
    delay(2500);
  // Red
    digitalWrite( B, LOW);
    delay(2500);
}
void smoothChange() {
  // White
  for( int i = 0; i <= 255; i++ ){
    analogWrite( R, i);
    analogWrite( G, i);
    analogWrite( B, i);
    Rlumi = i;
    Glumi = i;
    Blumi = i;
    delay(10);
  }
  // Yellow
  for( int i = 255; i >= 0; i-- ){
    analogWrite( B, i);
    delay(10);
    Blumi = i;
  }
  // Green
  for( int i = 255; i >= 0; i-- ){
    analogWrite( R, i);
    Rlumi = i;
    delay(10);
  }
  // Light Blue
  for( int i = 0; i <= 255; i++ ){
    analogWrite( B, i);
    Blumi = i;
    delay(10);
  }
  // Deep Blue
  for( int i = 255; i >= 0; i-- ){
    analogWrite( G, i);
    Glumi = i;
    delay(10);
  }  
  // Purple
  for( int i = 0; i <= 255; i++ ){
    analogWrite( R, i);
    Rlumi = i;
    delay(10);
  }
  // Red
  for( int i = 255; i >= 0; i-- ){
    analogWrite( B, i);
    Blumi = i;
    delay(10);
  }  
  // Nothing
  for( int i = 255; i >= 0; i-- ){
    analogWrite( R, i);
    Rlumi = i;
    delay(10);
  }
}
// BCD to DEC
uint8_t bcdTodec(uint8_t val){
  return ((val / 16 * 10) + (val % 16));
}
// read time from DS3231
uint16_t getTime(){
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS, 3);
  uint16_t curTime, m, s;  // minutes, seconds
  s = bcdTodec(Wire.read() & 0x7f);
  m = bcdTodec(Wire.read());
  curTime = bcdTodec(Wire.read() & 0x7f);
  curTime = (curTime * 100) + m;
  return curTime;
}
// read luminance from max44009
float getLumi(){
  unsigned int data[2] = {};
  // Start I2C Transmission
  Wire.beginTransmission(lightAddr);
  // Select data register
  Wire.write(0x03);
  // Stop I2C transmission
  Wire.endTransmission();
  // Request 2 bytes of data
  Wire.requestFrom(lightAddr, 2);
  // Read 2 bytes of data
  // luminance msb, luminance lsb
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  // Convert the data to lux
  int exponent = (data[0] & 0xF0) >> 4;
  int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
  float luminance = pow(2, exponent) * mantissa * 0.045;
  
  return luminance;
}


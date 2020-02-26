#include <SoftwareSerial.h>
#include <Wire.h>
// RX TX pin of RC device
#define RXD 3
#define TXD 4
// led pins
#define R 9
#define G 6
#define B 5
// DS3231 and time configuration
#define I2C_ADDRESS 0x68
#define startTime 1630
#define stopTime 2330
// MAX44009 I2C address is 0x4A(74)
#define lightAddr 0x4A

// open light code
const char code[4] = {'O','P','E','N'};

SoftwareSerial RC( TXD, RXD); // RX, TX

uint8_t bcdTodec(uint8_t val);
uint16_t getTime();
float getLumi();
void smoothChange();
void(* resetFunc) (void) = 0;

void setup() {
  RC.begin(9600);
  Wire.begin();
  pinMode( R, OUTPUT);
  pinMode( G, OUTPUT);
  pinMode( B, OUTPUT);
  Wire.beginTransmission(lightAddr);
  // Select configuration register
  Wire.write(0x02);
  // Continuous mode, Integration time = 800 ms
  Wire.write(0x80);
  // Stop I2C transmission
  Wire.endTransmission();
}


void loop() {
  uint8_t j = 0;
  char readCode[4] = {};
  while(RC.available()){
    readCode[j] = RC.read();
    delay(2);
    j++;
    if ( j == 4 ){
      j = 0;
      for( int i = 0; i < 4; i++ ){
        if( readCode[i] == code[i] ){
          j++;
        }
        if( j == 4 ){
            uint16_t curTime = getTime();
            float lumi = getLumi();
            if( (curTime > startTime) && (curTime < stopTime) && (lumi <= 5) ){
              smoothChange();
            }
            resetFunc();
        }
      }
    }
  }
}

void smoothChange() {
  // White
  for( int i = 0; i <= 255; i++ ){
    analogWrite( R, i);
    analogWrite( G, i);
    analogWrite( B, i);
    delay(10);
  }
  // Yellow
  for( int i = 255; i >= 0; i-- ){
    analogWrite( B, i);
    delay(10);
  }
  // Green
  for( int i = 255; i >= 0; i-- ){
    analogWrite( R, i);
    delay(10);
  }
  // Light Blue
  for( int i = 0; i <= 255; i++ ){
    analogWrite( B, i);
    delay(10);
  }
  // Deep Blue
  for( int i = 255; i >= 0; i-- ){
    analogWrite( G, i);
    delay(10);
  }  
  // Purple
  for( int i = 0; i <= 255; i++ ){
    analogWrite( R, i);
    delay(10);
  }
  // Red
  for( int i = 255; i >= 0; i-- ){
    analogWrite( B, i);
    delay(10);
  }  
  // Nothing
  for( int i = 255; i >= 0; i-- ){
    analogWrite( R, i);
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


/*GR-KURUMI Sketch Template Version: V2.02*/
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define devAddr 0x68
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// radio module control pin
#define EN 11
// Pin 22,23,24 are assigned to RGB LEDs.
#define LED_RED    22 // LOW active
#define LED_GREEN  23 // LOW active
#define LED_BLUE   24 // LOW active
// Pin 9, 10 are tx rx pins
#define TX 10
#define RX 9

SoftwareSerial RC(TX,RX);   //RX TX

int8_t i = 0;
int16_t ax, ay, az;
float lastY[3] = {};
float lastZ[3] = {};

void getAcceleration( int16_t &x, int16_t &y, int16_t &z);
bool calculateAbsDiff( float x, float y, float scale);

void setup() {
	RC.begin(9600);
    Wire.begin();
    // wake up
    Wire.beginTransmission(devAddr);	// 開啟MPU6050的傳輸
    Wire.write(PWR_MGMT_1);     		// 指定暫存器地址
    Wire.write(0);              		// 寫入一個位元組的數據
    Wire.endTransmission(true); 		// 結束傳輸，true表示釋放匯流排
    // set to cycle and disable temperature sensor and set clock source
    Wire.beginTransmission(devAddr);
    Wire.write(PWR_MGMT_1);
    Wire.write(B00101001);
    Wire.endTransmission(true);
    // set gyro scale
    Wire.beginTransmission(devAddr);
    Wire.write(GYRO_CONFIG);
    Wire.write(B00000000);
    Wire.endTransmission(true);
    // set range to +4g/-4g
    Wire.beginTransmission(devAddr);
    Wire.write(ACCEL_CONFIG);
    Wire.write(B00001000);
    Wire.endTransmission(true);
    // set wake frequency
    Wire.beginTransmission(devAddr);
    Wire.write(PWR_MGMT_2);
    Wire.write(B01000111);
    Wire.endTransmission(true);
    setPowerManagementMode(PM_STOP_MODE);	//set KURUMI to STOP_MODE
    // initialize radio module control pin
	pinMode(EN,OUTPUT);
    // initialize led pins
    pinMode(LED_RED, OUTPUT);
  	pinMode(LED_GREEN, OUTPUT);
  	pinMode(LED_BLUE, OUTPUT);
	// turn the LEDs on, glow white.
	digitalWrite(LED_RED, LOW);
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);
	delay(300);
	
	digitalWrite(LED_RED, HIGH);   // turn the RED LED off, glow sky blue.
    delay(300);                   // wait 200ms

    digitalWrite(LED_RED, LOW);    // turn the RED LED on
    digitalWrite(LED_GREEN, HIGH); // turn the GREEN LED off, glow pink.
    delay(300);                   // wait 200ms

    digitalWrite(LED_GREEN, LOW);  // turn the GREEN LED on
    digitalWrite(LED_BLUE, HIGH);  // turn the BLUE LED off, glow yellow.
    delay(300);                   // wait 300ms
    
	digitalWrite(LED_RED, HIGH);
	digitalWrite(LED_GREEN, HIGH);
	digitalWrite(LED_BLUE, HIGH);
	// initialize RC module
	RC.print("Initialize\n");
}

void loop() {
	// radio module to energy saving mode
	digitalWrite(EN, HIGH);
    // read raw accel measurements from device
    getAcceleration( ax, ay, az);
    float readAy = ay;
    float readAz = az;
    readAy = (readAy / 32768) * 39.2;
    readAz = (readAz / 32768) * 39.2;
    
    if( (calculateAbsDiff( lastY[i], readAy, 0.3) | calculateAbsDiff( lastZ[i], readAz, 0.3)) && readAy > 0  && readAz > 0 ){
      delay(50);
      getAcceleration( ax, ay, az);
      readAy = ay;
      readAz = az;
      readAy = (readAy / 32768) * 39.2;
      readAz = (readAz / 32768) * 39.2;
      if( (calculateAbsDiff( lastY[i], readAy, 0.1) | calculateAbsDiff( lastZ[i], readAz, 0.1)) && readAy > 0 || readAz > 0 ){
      	digitalWrite( EN, LOW);
      	delay(1);
        RC.print("OPEN\n");
        digitalWrite( LED_GREEN, LOW);
        delay(500);
      } 
    }
    else{
      lastY[i] = readAy;
      lastZ[i] = readAz;
    }
    
    i ++;
    if(i == 3){
      i = 0;
    }
    
    digitalWrite( LED_GREEN, HIGH);
    delay(50);
}

void getAcceleration( int16_t &x, int16_t &y, int16_t &z){
  Wire.beginTransmission(devAddr);		// 開啟MPU6050的傳輸
  Wire.write(ACCEL_XOUT_H); 			// 指定暫存器地址
  Wire.requestFrom(devAddr, 6, true); 	// 將輸據讀出到暫存
  Wire.endTransmission(true);			// 關閉傳輸模式
  x = Wire.read() << 8 | Wire.read();	// 兩個位元組，組成一個16位整數
  y = Wire.read() << 8 | Wire.read();	// 兩個位元組，組成一個16位整數
  z = Wire.read() << 8 | Wire.read();	// 兩個位元組，組成一個16位整數
}

bool calculateAbsDiff( float x, float y, float scale){
    x = abs(x);
    y = abs(y);
    float z;
    z = x - y;
    z = abs(z);
    if( z > scale ){
      return true;
    }
    else{
      return false;
    }
}
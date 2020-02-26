#include <Wire.h>
#define devAddr 0x68
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
int8_t i = 0;
int16_t ax, ay, az;
float lastY[3] = {};
float lastZ[3] = {};

void getAcceleration( int16_t* x, int16_t* y, int16_t* z);
bool calculateAbsDiff(float x, float y);

void setup() {
    Wire.begin();
    //wake up
    Serial.begin(38400);
    Wire.beginTransmission(devAddr); //开启MPU6050的传输
    Wire.write(PWR_MGMT_1);     //指定寄存器地址
    Wire.write(0);              //写入一个字节的数据
    Wire.endTransmission(true); //结束传输，true表示释放总线
    //set to cycle and disable temperature sensor and set clock source
    Wire.beginTransmission(devAddr);
    Wire.write(PWR_MGMT_1);
    Wire.write(B00101001);
    Wire.endTransmission(true);
    //set gyro scale
    Wire.beginTransmission(devAddr);
    Wire.write(GYRO_CONFIG);
    Wire.write(B00000000);
    Wire.endTransmission(true);
    //set range to +4g/-4g
    Wire.beginTransmission(devAddr);
    Wire.write(ACCEL_CONFIG);
    Wire.write(B00001000);
    Wire.endTransmission(true);
    //set wake frequency
    Wire.beginTransmission(devAddr);
    Wire.write(PWR_MGMT_2);
    Wire.write(B01000111);
    Wire.endTransmission(true);
    pinMode(13,OUTPUT);
}

void loop() {
    // read raw accel measurements from device
    getAcceleration(&ax, &ay, &az);
    float readAy = ay;
    float readAz = az;
    readAy = (readAy / 32768) * 39.2;
    readAz = (readAz / 32768) * 39.2;
    
    Serial.print(readAy); Serial.print("\t");
    Serial.print(readAz); Serial.print("\n");
    Serial.print(calculateAbsDiff(lastY[i], readAy)); Serial.print("\t");
    Serial.print(calculateAbsDiff(lastZ[i], readAz)); Serial.print("\n");
    Serial.print("\n");
    bool next = 0;
    
    if( ((calculateAbsDiff(lastY[i], readAy) | calculateAbsDiff(lastZ[i], readAz)) & readAy > 0  & readAz > 0) ){
      delay(50);
      getAcceleration(&ax, &ay, &az);
      readAy = ay;
      readAz = az;
      readAy = (readAy / 32768) * 39.2;
      readAz = (readAz / 32768) * 39.2;
      if( ((calculateAbsDiff(lastY[i], readAy) | calculateAbsDiff(lastZ[i], readAz)) & readAy > 0 & readAz > 0) ){
        digitalWrite( 13, HIGH);
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
    digitalWrite( 13, LOW);
    delay(50);
}
void getAcceleration( int16_t* x, int16_t* y, int16_t* z){
  Wire.beginTransmission(devAddr); //开启MPU6050的传输
  Wire.write(ACCEL_XOUT_H); //指定寄存器地址
  Wire.requestFrom(devAddr, 6, true); //将输据读出到缓存
  Wire.endTransmission(true); //关闭传输模式
  *x = Wire.read() << 8 | Wire.read(); //两个字节组成一个16位整数
  *y = Wire.read() << 8 | Wire.read(); //两个字节组成一个16位整数
  *z = Wire.read() << 8 | Wire.read(); //两个字节组成一个16位整数
}
bool calculateAbsDiff(float x, float y){
    x = abs(x);
    y = abs(y);
    float z;
    z = x - y;
    z = abs(z);
    if( z > 0.8 ){
      return true;
    }
    else{
      return false;
    }
}

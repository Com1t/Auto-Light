#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

MPU6050 accel;

int8_t i = 0;
int16_t ax, ay, az;
float lastY[3] = {};
float lastZ[3] = {};

float calculateAbsDiff(float &x, float &y);

void setup() {
    Wire.begin();
    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);
    // initialize device
    Serial.println("Initializing I2C devices...");
    accel.initialize();
    //set range to +4g/-4g
    accel.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
    Serial.println(accel.getFullScaleAccelRange());
    //setting to power saving configuration
    accel.setWakeCycleEnabled(2);
    accel.setTempSensorEnabled(0);
    accel.setWakeFrequency(1);
    accel.setStandbyXGyroEnabled(1);
    accel.setStandbyYGyroEnabled(1);
    accel.setStandbyZGyroEnabled(1);
    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accel.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    pinMode(13,OUTPUT);
}

void loop() {
    // read raw accel measurements from device
    accel.getAcceleration(&ax, &ay, &az);
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
    
    if( calculateAbsDiff(lastY[i], readAy) > 1.00 && calculateAbsDiff(lastZ[i], readAz) > 1.00 && readAy > 0  && readAz > 0 ){
      lastY[i] = readAy;
      lastZ[i] = readAz;
      next = 1;
      i ++;
      if(i == 3){
        i = 0;
      }
      delay(50);
    }
    
    if( next ){
      accel.getAcceleration(&ax, &ay, &az);
      readAy = ay;
      readAz = az;
      readAy = (readAy / 32768) * 39.2;
      readAz = (readAz / 32768) * 39.2;
      if( calculateAbsDiff(lastY[i], readAy) > 1.00 && calculateAbsDiff(lastZ[i], readAz) > 1.00 && readAy > 0 && readAz > 0 ){
        digitalWrite( 13, HIGH);
        delay(500);
      }
    }
    digitalWrite( 13, LOW);
    lastY[i] = readAy;
    lastZ[i] = readAz;
    i ++;
    if(i == 3){
      i = 0;
    }
    delay(50);
}

float calculateAbsDiff(float &x, float &y){
    x = abs(x);
    y = abs(y);
    float z;
    z = x - y;
    z = abs(z);
    return z;
}

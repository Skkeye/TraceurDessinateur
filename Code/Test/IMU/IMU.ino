#include <Arduino.h>
#include <Wire.h>

#define MPU_ADDRESS 0x68 // I2C address of the MPU-6050 sensor
#define ACCEL_SCALE 16384.0 // Accelerometer scale factor for +/-2g range

void setup() {
  Serial.begin(115200);
  Wire.begin();
  MPU_Init();
}

void loop() {
  // Read accelerometer data
  int16_t accel_x_raw, accel_y_raw, accel_z_raw;
  Read_Accel(&accel_x_raw, &accel_y_raw, &accel_z_raw);

  // Convert raw accelerometer data to G's
  float accel_x = (float)accel_x_raw / ACCEL_SCALE;
  float accel_y = (float)accel_y_raw / ACCEL_SCALE;
  float accel_z = (float)accel_z_raw / ACCEL_SCALE;

  // Print accelerometer data to serial monitor
  Serial.print("Accelerometer (G's): ");
  Serial.print(accel_x);
  Serial.print(", ");
  Serial.print(accel_y);
  Serial.print(", ");
  Serial.println(accel_z);

  delay(100);
}

void MPU_Init() {
  // Initialize MPU-6050 sensor
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B); // Power management register
  Wire.write(0); // Wake up sensor (0=normal mode)
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x1C); // Accelerometer configuration register
  Wire.write(0); // Set range to +/-2g
  Wire.endTransmission(true);
}

void Read_Accel(int16_t* accel_x, int16_t* accel_y, int16_t* accel_z) {
  // Read accelerometer data from MPU-6050 sensor
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B); // Starting register address for accelerometer data
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 6);   //(MPU_ADDRESS, 6, true); // Request 6 bytes of data

  // Combine high and low bytes to get 16-bit accelerometer data
  *accel_x = (Wire.read() << 8) | Wire.read();
  *accel_y = (Wire.read() << 8) | Wire.read();
  *accel_z = (Wire.read() << 8) | Wire.read();
}

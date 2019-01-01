/*
  Connection Pins
  INT to 2
  SDA to A4
  SCL to A5
  GND to GND
  VCC to 5V
  ---------

  You can use this code as you like but I do not accept any responsibility, I hope it can be usefull to you
  
*/

#include <Wire.h>
#define PI 3.1415926535897932384626433832795
/*Offset registers to be used*/
#define XA_OFFSET_H      0x06
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B
#define XG_OFFS_USRH     0x13
#define XG_OFFS_USRL     0x14
#define YG_OFFS_USRH     0x15
#define YG_OFFS_USRL     0x16
#define ZG_OFFS_USRH     0x17
#define ZG_OFFS_USRL     0x18
const int MPU = 0x68; /* constant int which stores I2C address */
int16_t accelerometer_x, accelerometer_y, accelerometer_z, gyro_x, gyro_y, gyro_z, temperature; /* variables to store read values */
int accelerometer_x_mean, accelerometer_y_mean, accelerometer_z_mean, gyro_x_mean, gyro_y_mean, gyro_z_mean;
int accelerometer_x_offset, accelerometer_y_offset, accelerometer_z_offset, gyro_x_offset, gyro_y_offset, gyro_z_offset;
int accelerometer_x_offsetH, accelerometer_x_offsetL, accelerometer_y_offsetH, accelerometer_y_offsetL, accelerometer_z_offsetH, accelerometer_z_offsetL, gyro_x_offsetH, gyro_x_offsetL, gyro_y_offsetH, gyro_y_offsetL, gyro_z_offsetH, gyro_z_offsetL;
int buffered_sample_size = 1000;
int accelerometer_sensivity = 4;/*Lower if you want a more sensitive calibration but it will take longer to calibrate it.*/
int gyro_sensivity = 1;
double accelerometer_x_angle = 0, accelerometer_y_angle = 0, accelerometer_z_angle = 0;
double gyro_x_angle_scaled = 0, gyro_y_angle_scaled = 0, gyro_z_angle_scaled = 0;
double gyro_x_angle = 0, gyro_y_angle = 0, gyro_z_angle = 0;
uint8_t temp_x_lb = 0, temp_y_lb = 0, temp_z_lb = 0; /*to store temperature bit*/
uint8_t mask = 1u; /*mask to use while getting temperature bit*/
double timeStep, timeCurrent, timePrev;

void setup() {

  Serial.begin(9600);

  Wire.begin();

  Serial.println("Starting MPU6050....");

  writer(0x6B, 0x00);

  Serial.println("Setting DLPF on MPU6050.... (0x06)");

  writer(0x1A, 0x06);

  Serial.println("Using full scale range of the gyroscope (1B register) on MPU6050 as it is.... (0x00)");

  writer(0x1B, 0x00); /*± 250 °/s is the most sensitive setting for gyro. when the range is narrower sensivity is increased.*/

  Serial.println("Using full scale range of the accelerometer (1C register) on MPU6050 as it is.... (0x00)");

  writer(0x1C, 0x00); /*± 2g is the most sensitive setting for accelerometer. when the range is narrower sensivity is increased.*/

  Serial.println("Reading Accelerometer Low Byte Registers to preserve bit 0s.... ");

  /*Gyro registers boot up with 0 by default, Accelerometer registers contains OTP values of the Accel factory trim, so they are not equals to 0,
    they have to be read and offset correcting values need to be applied to the read values also low bytes of each axis bit 0 is reserved and need to be preserved.*/

  temp_x_lb = read_spec_reg(XA_OFFSET_L_TC); /*read value*/

  temp_x_lb = temp_x_lb & mask; /*save temerature bit*/

  temp_y_lb = read_spec_reg(YA_OFFSET_L_TC); /*read value*/

  temp_y_lb = temp_y_lb & mask; /*save temerature bit*/

  temp_z_lb = read_spec_reg(ZA_OFFSET_L_TC); /*read value*/

  temp_z_lb = temp_z_lb & mask; /*save temerature bit*/

  /*writer(XA_OFFSET_H, 0x00);

    writer(XA_OFFSET_L_TC, 0x00);

    writer(YA_OFFSET_H, 0x00);

    writer(YA_OFFSET_L_TC, 0x00);

    writer(ZA_OFFSET_H, 0x00);

    writer(ZA_OFFSET_L_TC, 0x00);*/

  Serial.println("Setting Gyro Offset Registers to zero.... (0x00)");

  writer(XG_OFFS_USRH, 0x00);

  writer(XG_OFFS_USRL, 0x00);

  writer(YG_OFFS_USRH, 0x00);

  writer(YG_OFFS_USRL, 0x00);

  writer(ZG_OFFS_USRH, 0x00);

  writer(ZG_OFFS_USRL, 0x00);

  Serial.println("Setup completed!");

  delay(300);

  Serial.println("\nDo not touch or move sensor until completed!");

  Serial.print("\nCalculating mean...");

  calculate_mean();

  delay(100);

  Serial.print("\nCalculating offsets...");

  calculate_offset();

  delay(100);

  Serial.print("\nOffset values are set - Calibration is completed!");

  timeCurrent = millis();

}

void writer(int reg, int val) {

  /*Writer funtion to write value to the given register*/

  Wire.beginTransmission(MPU);

  Wire.write(reg);

  Wire.write(val);

  Wire.endTransmission();

}

int read_spec_reg(int reg) {

  /*reader funtion to read given register*/

  Wire.beginTransmission(MPU); /* begins the transmission to the I2C slave device with the given 7 -*/
  /*- bit address */

  Wire.write(reg); /* points to given register */

  Wire.endTransmission(false); /* when used with false flag it means reset instead of stop*/

  Wire.requestFrom(MPU, 1, true); /* request from <MPU> addressed slave <1> amount of bytes and <true> -*/
  /*- parameter has the same effect with endTransmission which is to stop and release theI2C bus -*/
  /*- requested bytes above can be read. Each register is 1 byte */

  return Wire.read(); /*returns the read value to the requester*/

}

void reader() {

  /*reader funtion to read Temperature, Accelerometer X, Y, Z and Gyro X, Y, Z values*/

  Wire.beginTransmission(MPU); /* begins the transmission to the I2C slave device with the given 7 -*/
  /*- bit address */

  Wire.write(0x3B); /* send start with register 0x3B ACCEL_XOUT_H */

  Wire.endTransmission(false); /* when used with false flag it means reset instead of stop*/

  Wire.requestFrom(MPU, 14, true); /* request from <MPU> addressed slave <14> amount of bytes and <true> -*/
  /*- parameter has the same effect with endTransmission which is to stop and release theI2C bus -*/
  /*- requested bytes above can be read. two registers are read and assigned to the same variable. since -*/
  /*- each register is 1 byte, 14 bytes requested will be enuoght for 7 variables */

  accelerometer_x = Wire.read() << 8 | Wire.read(); /* read registers ACCEL_XOUT_H 0x3B and ACCEL_XOUT_L 0x3C */

  accelerometer_y = Wire.read() << 8 | Wire.read(); /* read registers ACCEL_YOUT_H 0x3D and ACCEL_YOUT_L 0x3E */

  accelerometer_z = Wire.read() << 8 | Wire.read(); /* read registers ACCEL_ZOUT_H 0x3F and ACCEL_ZOUT_L 0x40 */

  temperature = Wire.read() << 8 | Wire.read(); /* read registers TEMP_OUT_H 0x41 and TEMP_OUT_L 0x42 */

  gyro_x = Wire.read() << 8 | Wire.read(); /* read registers GYRO_XOUT_H 0x43 and GYRO_XOUT_L 0x44 */

  gyro_y = Wire.read() << 8 | Wire.read(); /* read registers GYROXOUT_H 0x45 and  GYRO_XOUT_L 0x46 */

  gyro_z = Wire.read() << 8 | Wire.read(); /* read registers GYRO_XOUT_H 0x47 and  GYRO_XOUT_L 0x48 */

}

void calculate_mean() {

  /*set each variables to 0 before using*/
  long i = 0, accelerometer_x_buffered = 0, accelerometer_y_buffered = 0, accelerometer_z_buffered = 0, gyro_x_buffered = 0, gyro_y_buffered = 0, gyro_z_buffered = 0;

  while (i < (buffered_sample_size + 101)) { /*need to read defined buffer size + 101 since first 100 is discarded*/

    // read raw accel/gyro measurements from device
    reader();

    if (i > 100 && i <= (buffered_sample_size + 100)) { /*First 100 measures are discarded*/

      accelerometer_x_buffered = accelerometer_x_buffered + accelerometer_x;

      accelerometer_y_buffered = accelerometer_y_buffered + accelerometer_y;

      accelerometer_z_buffered = accelerometer_z_buffered + accelerometer_z;

      gyro_x_buffered = gyro_x_buffered + gyro_x;

      gyro_y_buffered = gyro_y_buffered + gyro_y;

      gyro_z_buffered = gyro_z_buffered + gyro_z;

    }

    if (i == (buffered_sample_size + 100)) { /*calculate mean values*/

      accelerometer_x_mean = accelerometer_x_buffered / buffered_sample_size;

      accelerometer_y_mean = accelerometer_y_buffered / buffered_sample_size;

      accelerometer_z_mean = accelerometer_z_buffered / buffered_sample_size;

      gyro_x_mean = gyro_x_buffered / buffered_sample_size;

      gyro_y_mean = gyro_y_buffered / buffered_sample_size;

      gyro_z_mean = gyro_z_buffered / buffered_sample_size;

    }

    i++;

    delay(2); //to avoid same value reading from fifo

  }

}

void calculate_offset() {

  accelerometer_x_offsetH = 0;

  accelerometer_x_offsetL = 0;

  accelerometer_y_offsetH = 0;

  accelerometer_y_offsetL = 0;

  accelerometer_z_offsetH = 0;

  accelerometer_z_offsetL = 0;

  gyro_x_offsetH = 0;

  gyro_x_offsetL = 0;

  gyro_y_offsetH = 0;

  gyro_y_offsetL = 0;

  gyro_z_offsetH = 0;

  gyro_z_offsetL = 0;

  accelerometer_x_offset = -accelerometer_x_mean / 8; /*accelerometer offset registers accepts ±16g which is 2048 LSB/g while having ±2g 16384 LSB/g so basically 16384/4=2048*/

  accelerometer_y_offset = -accelerometer_y_mean / 8; /*accelerometer offset registers accepts ±16g which is 2048 LSB/g while having ±2g 16384 LSB/g so basically 16384/4=2048*/

  accelerometer_z_offset = (16384 - accelerometer_z_mean) / 8; /*16384 is the expected amount of 1G which is the default value so need to substract it first and apply 16384/4=2048 formula*/

  gyro_x_offset = -gyro_x_mean / 4; /*gyro offset registers accepts ± 1000 °/s which is 32.8 LSB/°/s while having ± 250 °/s 131 LSB/°/s so basically 131/4=32.8*/

  gyro_y_offset = -gyro_y_mean / 4; /*gyro offset registers accepts ± 1000 °/s which is 32.8 LSB/°/s while having ± 250 °/s 131 LSB/°/s so basically 131/4=32.8*/

  gyro_z_offset = -gyro_z_mean / 4; /*gyro offset registers accepts ± 1000 °/s which is 32.8 LSB/°/s while having ± 250 °/s 131 LSB/°/s so basically 131/4=32.8*/

  while (1) {

    int passCounter = 0;

    accelerometer_x_offsetH = accelerometer_x_offset >> 8; /*shift to remove low bit part*/

    accelerometer_x_offsetL = accelerometer_x_offset & 0x00FF; /*and with 0x00FF to remove high bit part*/

    accelerometer_x_offsetL = accelerometer_x_offset | temp_x_lb; /*set temperature bit to low bit axis*/

    accelerometer_y_offsetH = accelerometer_y_offset >> 8;

    accelerometer_y_offsetL = accelerometer_y_offset & 0x00FF;

    accelerometer_y_offsetL = accelerometer_y_offsetL | temp_y_lb;

    accelerometer_z_offsetH = accelerometer_z_offset >> 8;

    accelerometer_z_offsetL = accelerometer_z_offset & 0x00FF;

    accelerometer_z_offsetL = accelerometer_z_offsetL | temp_z_lb;

    gyro_x_offsetH = gyro_x_offset >> 8;

    gyro_x_offsetL = gyro_x_offset & 0x00FF;

    gyro_y_offsetH = gyro_y_offset >> 8;

    gyro_y_offsetL = gyro_y_offset & 0x00FF;

    gyro_z_offsetH = gyro_z_offset >> 8;

    gyro_z_offsetL = gyro_z_offset & 0x00FF;

    writer(XA_OFFSET_H, accelerometer_x_offsetH);

    writer(XA_OFFSET_L_TC, accelerometer_x_offsetL);

    writer(YA_OFFSET_H, accelerometer_y_offsetH);

    writer(YA_OFFSET_L_TC, accelerometer_y_offsetL);

    writer(ZA_OFFSET_H, accelerometer_z_offsetH);

    writer(ZA_OFFSET_L_TC, accelerometer_z_offsetL);

    writer(XG_OFFS_USRH, gyro_x_offsetH);

    writer(XG_OFFS_USRL, gyro_x_offsetL);

    writer(YG_OFFS_USRH, gyro_y_offsetH);

    writer(YG_OFFS_USRL, gyro_y_offsetL);

    writer(ZG_OFFS_USRH, gyro_z_offsetH);

    writer(ZG_OFFS_USRL, gyro_z_offsetL);

    calculate_mean();

    Serial.print(".");

    if (abs(accelerometer_x_mean) <= accelerometer_sensivity) {/*to check if mean result is within sensivity range*/
      passCounter++; /*if within range increment passCounter*/
    }
    else { /*else calculate offset*/
      accelerometer_x_offset = accelerometer_x_offset - accelerometer_x_mean / 8; /*accelerometer offset registers accepts ±16g which is 2048 LSB/g while having ±2g 16384 LSB/g so basically 16384/4=2048*/
    }

    if (abs(accelerometer_y_mean) <= accelerometer_sensivity) {
      passCounter++;
    }
    else {
      accelerometer_y_offset = accelerometer_y_offset - accelerometer_y_mean / 8;
    }

    if (abs(16384 - accelerometer_z_mean) <= accelerometer_sensivity) {
      passCounter++;
    }
    else {
      accelerometer_z_offset = accelerometer_z_offset + (16384 - accelerometer_z_mean) / 8; /*16384 is the expected amount of 1G*/
    }

    if (abs(gyro_x_mean) <= gyro_sensivity) {
      passCounter++;
    }
    else {
      gyro_x_offset = gyro_x_offset - gyro_x_mean / 4; /*gyro offset registers accepts ± 1000 °/s which is 32.8 LSB/°/s while having ± 250 °/s 131 LSB/°/s so basically 131/4=32.8*/
    }

    if (abs(gyro_y_mean) <= gyro_sensivity) {
      passCounter++;
    }
    else {
      gyro_y_offset = gyro_y_offset - gyro_y_mean / 4;
    }

    if (abs(gyro_z_mean) <= gyro_sensivity) {
      passCounter++;
    }
    else {
      gyro_z_offset = gyro_z_offset - gyro_z_mean / 4;
    }
    if (passCounter == 6) break; /*if all of the above mean results ok break*/

  }

  Serial.println("");

}

void loop() {

  timePrev = timeCurrent;

  timeCurrent = millis();

  timeStep = (timeCurrent - timePrev) / 1000;

  reader();

  gyro_x_angle_scaled = (gyro_x / 131) * timeStep; /* divided by LSB Sensivity times time seperation to scale */

  gyro_y_angle_scaled = (gyro_y / 131) * timeStep;

  /*MPU6050 does not have a magnetometer so expecting a correct yaw is no very realistic*/

  /*gyro_z_angle_scaled = (gyro_z / 131) * timeStep;*/

  accelerometer_x_angle = (180 / PI) * atan2f(accelerometer_y, accelerometer_z);

  accelerometer_y_angle = (180 / PI) * atan2f(accelerometer_x, accelerometer_z);

  /*accelerometer_z_angle = (180 / PI) * atan2f(accelerometer_y, accelerometer_x);*/

  gyro_x_angle = gyro_x_angle + gyro_x_angle_scaled;

  gyro_y_angle = gyro_y_angle + gyro_y_angle_scaled;

  /*gyro_z_angle = gyro_z_angle + gyro_z_angle_scaled;*/

  gyro_x_angle = (0.02 * accelerometer_x_angle) + (0.98 * gyro_x_angle);

  gyro_y_angle = (0.02 * accelerometer_y_angle) + (0.98 * gyro_y_angle);

  /*gyro_z_angle = (0.02 * accelerometer_z_angle) + (0.98 * gyro_z_angle);*/

  Serial.print("Accelerometer in degrees : ");

  Serial.print(" X = "); Serial.print(accelerometer_x_angle);

  Serial.print(" | Y = "); Serial.print(accelerometer_y_angle);

  /*Serial.print(" | Z = "); Serial.println(accelerometer_z_angle);*/

  Serial.println(" ");

  Serial.print("Gyroscope in degrees : ");

  Serial.print(" X = "); Serial.print(gyro_x_angle);

  Serial.print(" | Y = "); Serial.print(gyro_y_angle);

  /*Serial.print(" | Z = "); Serial.println(gyro_z_angle);*/

  Serial.println(" ");

  Serial.print("Temperature: "); Serial.println(temperature / 340 + 36.53); /* MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2 page 30 of 46 */

  Serial.println(" ");

}

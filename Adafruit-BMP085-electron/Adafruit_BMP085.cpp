/***************************************************
  This is a library for the Adafruit BMP085/BMP180 Barometric Pressure + Temp sensor

  Designed specifically to work with the Adafruit BMP085 or BMP180 Breakout
  ----> http://www.adafruit.com/products/391
  ----> http://www.adafruit.com/products/1603

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "application.h"
#include "Adafruit_BMP085.h"

Adafruit_BMP085::Adafruit_BMP085() {
}


boolean Adafruit_BMP085::begin(uint8_t mode) {
  if (mode > BMP085_ULTRAHIGHRES)
    mode = BMP085_ULTRAHIGHRES;
  oversampling = mode;

  Wire.begin();

  if (read8(0xD0) != 0x55) return false;

  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);


  return true;
}

uint16_t Adafruit_BMP085::readRawTemperature(void) {
  write8(BMP085_CONTROL, BMP085_READTEMPCMD);
  delay(5);

  return read16(BMP085_TEMPDATA);
}

uint32_t Adafruit_BMP085::readRawPressure(void) {
  uint32_t raw;

  write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  if (oversampling == BMP085_ULTRALOWPOWER)
    delay(5);
  else if (oversampling == BMP085_STANDARD)
    delay(8);
  else if (oversampling == BMP085_HIGHRES)
    delay(14);
  else
    delay(26);

  raw = read16(BMP085_PRESSUREDATA);

  raw <<= 8;
  raw |= read8(BMP085_PRESSUREDATA+2);
  raw >>= (8 - oversampling);

 /* this pull broke stuff, look at it later?
  if (oversampling==0) {
    raw <<= 8;
    raw |= read8(BMP085_PRESSUREDATA+2);
    raw >>= (8 - oversampling);
  }
 */

  return raw;
}


int32_t Adafruit_BMP085::readPressure(void) {
  int32_t UT, UP, BB3, BB5, BB6, X1, X2, X3, p;
  uint32_t BB4, BB7;

  UT = readRawTemperature();
  UP = readRawPressure();

  // do temperature calculations
  X1=(UT-(int32_t)(ac6))*((int32_t)(ac5))/pow(2,15);
  X2=((int32_t)mc*pow(2,11))/(X1+(int32_t)md);
  BB5=X1 + X2;

  // do pressure calcs
  BB6 = BB5 - 4000;
  X1 = ((int32_t)b2 * ( (BB6 * BB6)>>12 )) >> 11;
  X2 = ((int32_t)ac2 * BB6) >> 11;
  X3 = X1 + X2;
  BB3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

  X1 = ((int32_t)ac3 * BB6) >> 13;
  X2 = ((int32_t)b1 * ((BB6 * BB6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  BB4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  BB7 = ((uint32_t)UP - BB3) * (uint32_t)( 50000UL >> oversampling );

  if (BB7 < 0x80000000) {
    p = (BB7 * 2) / BB4;
  } else {
    p = (BB7 / BB4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;

  p = p + ((X1 + X2 + (int32_t)3791)>>4);

  return p;
}


float Adafruit_BMP085::readTemperature(void) {
  int32_t UT, X1, X2, BB5;     // following ds convention
  float temp;

  UT = readRawTemperature();

  // step 1
  X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) / pow(2,15);
  X2 = ((int32_t)mc * pow(2,11)) / (X1+(int32_t)md);
  BB5 = X1 + X2;
  temp = (BB5+8)/pow(2,4);
  temp /= 10;

  return temp;
}

float Adafruit_BMP085::readAltitude(float sealevelPressure) {
  float altitude;
  float pressure = readPressure();
  altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));
  return altitude;
}


/*********************************************************************/

uint8_t Adafruit_BMP085::read8(uint8_t a) {
  uint8_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission

  Wire.requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
  ret = Wire.read(); // receive DATA

  return ret;
}

uint16_t Adafruit_BMP085::read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission

  Wire.requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA

  return ret;
}

void Adafruit_BMP085::write8(uint8_t a, uint8_t d) {
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device
  Wire.write(a); // sends register address to read from
  Wire.write(d);  // write data
  Wire.endTransmission(); // end transmission
}

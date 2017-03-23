/***************************************************
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  If you're using multiple sensors, or you just want to change the I2C address
  to something else, you can choose from
  5 different options - 0x28, 0x29 (default), 0x2A, 0x2B, 0x2C and 0x2D

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Additional fixes and enhancements made by Chuan Khoo:
  touchedAnalog outputs int8_t as delta readings
  exposed I2C recalibration calls to individual pin(s)
  exposed I2C standbyConfig call; see func below
  setInputEnable to enable/disable C1-8 during standby

  **** THIS LIBRARY ONLY IMPLEMENTS IC2 PROTOCOL ****
 ****************************************************/

#include "Adafruit_CAP1188.h"

uint8_t mySPCR, SPCRback;

Adafruit_CAP1188::Adafruit_CAP1188(int8_t resetpin) {
  // I2C
  _resetpin = resetpin;
  _i2c = true;
}

Adafruit_CAP1188::Adafruit_CAP1188(int8_t cspin, int8_t resetpin) {
  // Hardware SPI
  _cs = cspin;
  _resetpin = resetpin;
  _clk = -1;
  _i2c = false;
}

Adafruit_CAP1188::Adafruit_CAP1188(int8_t clkpin, int8_t misopin,
                                   int8_t mosipin, int8_t cspin,
                                   int8_t resetpin) {
  // Software SPI
  _cs = cspin;
  _resetpin = resetpin;
  _clk = clkpin;
  _miso = misopin;
  _mosi = mosipin;
  _i2c = false;
}

boolean Adafruit_CAP1188::begin(uint8_t i2caddr) {
  if (_i2c) {
    Wire.begin();

    _i2caddr = i2caddr;
  }

  if (_resetpin != -1) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW);
    delay(100);
    digitalWrite(_resetpin, HIGH);
    delay(100);
    digitalWrite(_resetpin, LOW);
    delay(100);
  }

  readRegister(CAP1188_PRODID);

  // Useful debugging info
  // Serial.print("Product ID: 0x");
  // Serial.println(readRegister(CAP1188_PRODID), HEX);
  // Serial.print("Manuf. ID: 0x");
  // Serial.println(readRegister(CAP1188_MANUID), HEX);
  // Serial.print("Revision: 0x");
  // Serial.println(readRegister(CAP1188_REV), HEX);

  if ((readRegister(CAP1188_PRODID) != 0x50) ||
      (readRegister(CAP1188_MANUID) != 0x5D) ||
      (readRegister(CAP1188_REV) != 0x83)) {
    return false;
  }
  // allow multiple touches (disable multi-touch lockout)
  writeRegister(CAP1188_MTBLK, 0);
  // Have LEDs follow touches
  writeRegister(CAP1188_LEDLINK, 0xFF);
  // speed up a bit

  // writeRegister(CAP1188_STANDBYCFG, 0x30);
  // we change sensitivity here
  // writeRegister(CAP188_SENSITIVYCONTROL, 0x50);

  // setSensitivity(1);
  // writeRegister(CAP188_SENSITIVYCONTROL, 0x70);
  // Serial.print("1Fh Sensitivity Control in BIN: ");
  // Serial.println(readRegister(CAP188_SENSITIVYCONTROL), BIN);

  // Serial.print("42h Standby Sensitivity: 0x");
  // Serial.println(readRegister(CAP1188_STANDBYSENSITIVITY), HEX);

  // Serial.print("MultiTouch: 0x");
  // Serial.println(readRegister(CAP1188_MTBLK), HEX);
  // BIT DECODE for number of samples taken
  // Serial.print("bit decode samples taken: 0x");
  // Serial.println(readRegister(CAP1188_STANDBYCFG), HEX);

  // writeRegister(CAP188_SENSITIVYCONTROL, 0x30);

  return true;
}

void Adafruit_CAP1188::setGain(uint8_t b) {
  writeRegister(CAP1188_MAIN, b);
}

void Adafruit_CAP1188::setGeneralConfig(uint8_t a, uint8_t b) {
  writeRegister(CAP1188_CONFIGA, a);
  writeRegister(CAP1188_CONFIGB, b);
}

void Adafruit_CAP1188::setInputConfig(uint8_t a, uint8_t b) {
  writeRegister(CAP1188_INPUTCONFIGA, a);
  writeRegister(CAP1188_INPUTCONFIGB, b);
}

void Adafruit_CAP1188::setActiveAvgSampConfig(uint8_t b) {
  // 8-bit register; active high
  writeRegister(CAP1188_ACTIVEAVGSAMP, b);
}

void Adafruit_CAP1188::setInputEnable(uint8_t b) {
  // 8-bit register; active high
  writeRegister(CAP1188_INPUTENABLE, b);
}

void Adafruit_CAP1188::setInputThreshold(uint8_t b) {
  // 8-bit register; active high
  writeRegister(CAP1188_INPUTTHRESH, b);
}

void Adafruit_CAP1188::setStandbyActive(uint8_t b) {
  // 8-bit register; active high
  writeRegister(CAP1188_STANDBYACTIVE, b);
}

void Adafruit_CAP1188::setSensitivity(int sensitivity) { // 1 to 8

  switch (sensitivity) {
  // least sensitive
  case 1:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x78);
    break;
  case 2:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x68);
    break;
  case 3:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x58);
    break;
  case 4:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x48);
    break;
  case 5:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x38);
    break;
  case 6:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x28);
    break;
  case 7:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x18);
    break;
  // most sensitive
  case 8:
    writeRegister(CAP188_SENSITIVYCONTROL, 0x8);
    break;
  }

  // Let's read how sensible the sensor is
  // Serial.print("Sensitivity in HEX: 0x");
  // Serial.println(readRegister(CAP188_SENSITIVYCONTROL), HEX);
}

void Adafruit_CAP1188::setRepeatEnable(uint8_t b) {
  writeRegister(CAP1188_RPTRATE, b);
}

uint8_t Adafruit_CAP1188::touched(void) {
  uint8_t t = readRegister(CAP1188_SENINPUTSTATUS); // we call the input status
                                                    // which will return yes and
                                                    // no values for all the
                                                    // elements
  if (t) {
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
  }
  return t; // re-write the main control register; interrupt flag needs to be
            // reset after each read (or subsequent reads won't work)
}

int8_t Adafruit_CAP1188::touchedAnalog(byte offset) {
  int8_t t = readRegister(CAP1188_ANALOGID + offset);
  if (t) {
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
  }
  return t; // re-write the main control register; interrupt flag needs to be
            // reset after each read (or subsequent reads won't work)
}

void Adafruit_CAP1188::LEDpolarity(uint8_t x) {
  writeRegister(CAP1188_LEDPOL, x);
}

void Adafruit_CAP1188::standbyConfig(uint8_t c) { // setup Standby configuration
  // default is 0x30, which disables avg summing, 8 samples/measurement,
  // 320us sample time, 35ms cycle time (pg 62 of datasheet)
  writeRegister(CAP1188_STANDBYCFG, c);
}

void Adafruit_CAP1188::recalibrate(void) { // no argument = recalibrate all pins
  for (uint8_t i = 1; i < 9; i++) {
    recalibrate(i);
  }
}

void Adafruit_CAP1188::recalibrate(uint8_t x) {
  // accepts 1-8; bitwise shift happens here
  uint8_t b = 1 << (x - 1);
  writeRegister(CAP1188_CALIBRATE, b);
  delay(600); // delay required as per datasheet (pg 54)
}

/*********************************************************************/

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) { return Wire.read(); }

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) { Wire.write((uint8_t)x); }

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t Adafruit_CAP1188::spixfer(uint8_t data) {
  if (_clk == -1) {
    // Serial.println("Hardware SPI");
    return SPI.transfer(data);
  } else {
    // Serial.println("Software SPI");
    uint8_t reply = 0;
    for (int i = 7; i >= 0; i--) {
      reply <<= 1;
      digitalWrite(_clk, LOW);
      digitalWrite(_mosi, data & (1 << i));
      digitalWrite(_clk, HIGH);
      if (digitalRead(_miso))
        reply |= 1;
    }
    return reply;
  }
}

uint8_t Adafruit_CAP1188::readRegister(uint8_t reg) {
  if (_i2c) {
    Wire.beginTransmission(_i2caddr);
    i2cwrite(reg);
    Wire.endTransmission();
    Wire.requestFrom(_i2caddr,
                     1); // requests the first value of the given address
    return (i2cread());
  }
}

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_CAP1188::writeRegister(uint8_t reg, uint8_t value) {
  if (_i2c) {
    Wire.beginTransmission(_i2caddr);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    Wire.endTransmission();
  }
}

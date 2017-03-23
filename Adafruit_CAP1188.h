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
  setActive to enable/disable C1-8 during standby

  **** THIS LIBRARY ONLY IMPLEMENTS IC2 PROTOCOL ****
 ****************************************************/

// We make sure we load the right header for the Photon
#ifdef PARTICLE
#include "application.h"
#endif

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ARDUINO
#include <Wire.h>
#include <SPI.h>
#endif

// The default I2C address
#define CAP1188_I2CADDR 0x29
// Wire a cable from the 3V to the AD pin in order to pass this address to the
// constructor
#define CAP1188_I2C2NDADDR 0x28

// Some registers we use
#define CAP1188_SENINPUTSTATUS 0x3
#define CAP1188_MTBLK 0x2A
#define CAP1188_LEDLINK 0x72
#define CAP1188_PRODID 0xFD
#define CAP1188_MANUID 0xFE
#define CAP1188_STANDBYCFG 0x41
#define CAP1188_REV 0xFF
#define CAP1188_MAIN 0x00   // (pg 41)
#define CAP1188_MAIN_INT 0x01
#define CAP1188_LEDPOL 0x73

/* chuank – newly added registers */

// ACTIVE mode
#define CAP1188_ACTIVEAVGSAMP 0x24   // averaging & sampling (pg 52)
#define CAP188_SENSITIVYCONTROL 0x1F // controls sensitivity (pg 45)

// STANDBY mode
#define CAP1188_STANDBYSENSITIVITY 0x42 // controls standby sensitivity (pg 63)
#define CAP1188_STANDBYACTIVE 0x40      // enables/disables C1-8 in standby

// configuration register addresses
#define CAP1188_CONFIGA 0x20       // general configuration 20h (pg 47)
#define CAP1188_CONFIGB 0x44       // general configuration 44h (pg 48)
#define CAP1188_INPUTENABLE 0x21   // enables/disables C1-8 in active (pg 49)
#define CAP1188_INPUTCONFIGA 0x22  // MAX_DUR/RPT_RATE (pg 49)
#define CAP1188_INPUTCONFIGB 0x23  // PRESS_HOLD time (pg 49)
#define CAP1188_INPUTTHRESH 0x30   // Sensor Input Threshold (pg 60)

#define CAP1188_ANALOGID 0x10     // returns analog values (0x10 - 0x17)
#define CAP1188_CALIBRATE 0x26    // force calibration
#define CAP1188_RPTRATE 0x28      // enables/disable press repeat status

class Adafruit_CAP1188 {
public:
  // Software SPI
  Adafruit_CAP1188(int8_t clkpin, int8_t misopin, int8_t mosipin, int8_t cspin,
                   int8_t resetpin);
  // Hardware SPI
  Adafruit_CAP1188(int8_t cspin, int8_t resetpin);
  // Hardware I2C
  Adafruit_CAP1188(int8_t resetpin = -1);

  boolean begin(uint8_t i2caddr = CAP1188_I2CADDR);
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t touched(void);
  int8_t touchedAnalog(byte offset);
  void LEDpolarity(uint8_t x);
  void setGain(uint8_t b);
  void setGeneralConfig(uint8_t a, uint8_t b);
  void setInputConfig(uint8_t a, uint8_t b);
  void setActiveAvgSampConfig(uint8_t b);
  void setInputEnable(uint8_t b);
  void setInputThreshold(uint8_t b);
  void setStandbyActive(uint8_t b);
  void setSensitivity(int sensitivity);
  void setRepeatEnable(uint8_t b);
  void recalibrate();
  void recalibrate(uint8_t x);
  void standbyConfig(uint8_t c);

private:
  uint8_t spixfer(uint8_t x);
  boolean _i2c;
  int8_t _i2caddr, _resetpin, _cs, _clk, _mosi, _miso;
};

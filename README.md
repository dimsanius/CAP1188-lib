# Introduction
This repo contains a library for CAP1188 touch capacitive sensor which can be controlled via SPI or I2C interface. It is designed for ESP using Arduino framework. Moreover, SPI interface designed in such a way that any arbitrary GPIO pin can be used as CS pin.

Submit bug reports and feature suggestions, or track changes in the [issue page](https://github.com/dimsanius/CAP1188-lib/issues)

## Table of contents

- Requirements
- Installation
- Configuration
- Maintainer

## Requirements

This library requires the following libraries to work:

- [I2Cdevlib](https://github.com/jrowberg/i2cdevlib) (located under `Arduino/I2Cdev`) _NOTE: plan is to move to native Wire library for simplicity of installation_
- [SPI](https://www.arduino.cc/reference/en/language/functions/communication/spi/) (this library is included natively into Arduino Framework, no need to install it separately)

## Installation

* Copy all contents of `src` folder into your project
* Include `CAP1188.h` within your project file

## Configuration
There are 2 different interfaces to communicate with CAP1188 - I2C or SPI (Normal - 4 wire and bidirectional - 3 wire. The latter is not implemented).
### I2C configuration
* Connect SCL and SDA/SDI pins from CAP1188 to according pins of your board
* Connect RST pin to any available GPIO
* Make sure that `CAP1188.h` is included within your project file
* Make sure `I2Cdevlib` files are located at the same folder as `CAP1188` files
* Create an object and initialise it as following:
```
CAP1188 cap1188_1;
cap1188_1.initI2C(CAP1188_I2C_ADDRESS, RESET_PIN);
```
_Possible I2C addresses are listed in `CAP1188.h`_

### SPI configuration
* Connect MISO, MOSI, SCK pins from CAP1188 to according pins of your board
* Connect CS pin to any available GPIO
* Connect RST pin to any available GPIO
* Make sure that `CAP1188.h` is included within your project file
* Create an object and initialise it as following:
```
CAP1188 cap1188_1;
cap1188_1.initSPI(CS_PIN, RESET_PIN);
```

### Test after initialisation
To test out if all connections are fine, make use of `getProductId()` as following:
```
// Assuming CAP1188 is already initilised via I2C and SPI interface
uint8_t productId;
productId = cap1188_1.getProductId();
Serial.print(productId); // This should print 80 (50h) on success
```

Most of the commands are written in such a way, that they will automatically detect if SPI or I2C was used during initialisation. Majority of functions start with `get` or `set`, so make use of them! Happy coding!
## Maintainer
- [dimsanius](https://github.com/dimsanius)
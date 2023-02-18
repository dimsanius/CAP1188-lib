#include <I2Cdev.h> // TODO: Move to Wire.h
#include <SPI.h>
#include "CAP1188.h"
#include "CAP1188_reg.h"

/**
 * @brief Instantiates a new CAP1188 class. Initialise it using *.initI2C(...) or *.initSPI(...)
 */
CAP1188::CAP1188(){};

/**
 * @brief Initilises CAP1188 module using I2C interface.
 * Following actions are done during initlisation:
 * 1. CAP1188 is reset by holding RESET pin HIGH
 * 2. CAP1188 is set to allow multiple touches at the same time
 * 3. CAP1188 buttons and LEDs are linked (corresponding LED turns on when corresponding button is pressed)
 * @param address I2C address of a device (possible options: 0x28, 0x29(defualt), 0x2A, 0x2B, 0x2C)
 * @param resetPin Chip RESET pin number
 * @returns void
 */
void CAP1188::initI2C(uint8_t address, uint8_t resetPin)
{
    _resetPin = resetPin;
    _i2cAddress = address;
    _i2c = true;

    pinMode(_resetPin, OUTPUT);

    // Reset CAP1188 module
    digitalWrite(_resetPin, HIGH);
    delay(10);
    digitalWrite(_resetPin, LOW);

    // Allow multiple touches
    setMultipleTouchConfiguration(0, 0);

    // Link LEDs and buttons
    setSensorInputLEDLinking(0b11111111);
}

/**
 * @brief Initilises CAP1188 module using SPI 4-wire (aka Normal mode) interface.
 * Following actions are done during initlisation:
 * 1. CAP1188 is reset by holding RESET pin HIGH
 * 2. CAP1188 is set to allow multiple touches at the same time
 * 3. CAP1188 buttons and LEDs are linked (corresponding LED turns on when corresponding button is pressed)
 * @param csPin CS pin of a device (must be any arbitrary GPIO pin)
 * @param resetPin Chip RESET pin number
 * @returns void
 */
void CAP1188::initSPI(uint8_t csPin, uint8_t resetPin)
{
    _resetPin = resetPin;
    _csPin = csPin;
    _i2c = false;

    pinMode(_resetPin, OUTPUT);
    pinMode(_csPin, OUTPUT);

    // Reset CAP1188 module
    digitalWrite(_resetPin, HIGH);
    delay(10);
    digitalWrite(_resetPin, LOW);
    resetSPI(); // Reset SPI interface of CAP1188 module

    // Allow multiple touches
    setMultipleTouchConfiguration(0, 0);

    // Link LEDs and buttons
    setSensorInputLEDLinking(0b11111111);
}

/**
 * @brief Reads & returns product ID of a module
 * @returns product ID of a module
 */
uint8_t CAP1188::getProductId()
{
    uint8_t productId;
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_PRODUCT_ID_REG, &productId);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_PRODUCT_ID_REG, &productId);
    }
    return productId;
}

/**
 * @brief Reads & returns manufacturer ID of a module
 * @returns manufacturer ID of a module
 */
uint8_t CAP1188::getManufacturerId()
{
    uint8_t manufacturerId;
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_MANUFACTURER_ID_REG, &manufacturerId);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_MANUFACTURER_ID_REG, &manufacturerId);
    }
    return manufacturerId;
}

/**
 * @brief Reads & returns revision of a module
 * @returns revision of a module
 */
uint8_t CAP1188::getRevision()
{
    uint8_t revision;
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_REVISION_REG, &revision);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_REVISION_REG, &revision);
    }
    return revision;
}

/**
 * @brief Writes data to Multiple Touch Configuration Register, that controls settings for the multiple touch detection circuitry.
 * @param multipleTouchCircuitryEnable multiple touch circuitry enable setting (true to turn it on, false to turn it off)
 * @param numberOfSimultaneousTouches number of total simultaneous touches (1..4). This setting has no effect
 *                                      if multipleTouchCircuitryEnable is set to false
 * @return true on success, false on error
 */
bool CAP1188::setMultipleTouchConfiguration(bool multipleTouchCircuitryEnable, uint8_t numberOfSimultaneousTouches)
{
    uint8_t buff = 0;
    // Checking if multiple touch circuitry is required
    if (multipleTouchCircuitryEnable)
    {
        // Set appropriate bits to allow multiple touches at the same time
        switch (numberOfSimultaneousTouches)
        {
        case 1:
            buff = 128;
            break;

        case 2:
            buff = 132;
            break;

        case 3:
            buff = 136;
            break;

        case 4:
            buff = 140;
            break;

        default:
            // Wrong amount of simultaneous touches specified
            return false;
            break;
        }
    }

    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAP1188_MULTIPLE_TOUCH_CONFIGURATION_REG, buff);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAP1188_MULTIPLE_TOUCH_CONFIGURATION_REG, buff);
    }
    return true;
}
/**
 * @brief Writes data to Sensor Input LED Linking Register, which controls whether a capacitive touch sensor input is linked to an LED output.
 * @param ledsToLink 8-bit data that links LEDs to buttons (i.e. 0b00001111 will turn LED linking on between L1-L4 and C1-C4)
 * @return true
 */
bool CAP1188::setSensorInputLEDLinking(uint8_t ledsToLink)
{
    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAP1188_SENSOR_INPUT_LED_LINKING_REG, ledsToLink);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_LED_LINKING_REG, ledsToLink);
    }
    return true;
}

/**
 * @brief Reads keys pressed from a module. I.e.
 * if key 1 & 3 pressed, returns b00000101 (5 in decimal)
 * @return keys pressed in binary format
 */
uint8_t CAP1188::getSensorInputs()
{
    uint8_t keys;
    // Reading keys
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_SENSOR_INPUT_STATUS_REG, &keys);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_STATUS_REG, &keys);
    }
    if (keys)
    {
        // Bring INT down
        // 1. Read current register state
        // 2. Modify INT bit (located at BIT0)
        // 3. Write register back
        uint8_t currentRegState;
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_MAIN_CONTROL_REG, &currentRegState);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_MAIN_CONTROL_REG, &currentRegState);
        }

        currentRegState = currentRegState & ~0x01;

        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_MAIN_CONTROL_REG, currentRegState);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_MAIN_CONTROL_REG, currentRegState);
        }
    }

    return keys;
}

/**
 * @brief Writes data to Standby Configuration Register, that controls averaging and cycle time while in standby.
 * @param averageSum Determines whether the active sensor inputs will average the programmed number of samples or whether they will accumulate for the programmed number of samples (use CAP1188_AVG_SUM_...)
 * @param samplesPerMeasurement Determines the number of samples that are taken for all active channels during the sensor cycle (use CAP1188_SAMPLES_PER_MEASUREMENT_...)
 * @param samplingTime Determines the time to take a single sample when the device is in Standby (use CAP1188_SAMPLING_TIME_...)
 * @param cycleTime Determines the overall cycle time for all measured channels during standby operation (use CAP1188_CYCLE_TIME_...)
 * @return true
 */
bool CAP1188::setStandbyConfiguration(uint8_t averageSum, uint8_t samplesPerMeasurement, uint8_t samplingTime, uint8_t cycleTime)
{
    uint8_t reg;
    reg = averageSum << 7 | samplesPerMeasurement << 4 | samplingTime << 2 | cycleTime;
    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAP1188_STANDBY_CONFIGURATION_REG, reg);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAP1188_STANDBY_CONFIGURATION_REG, reg);
    }

    return true;
}

/**
 * @brief Reads data from Standby Configuration Register, that controls averaging and cycle time while in standby.
 * @param averageSum buffer to receive "average or summation(accumulation)" in to
 * @param samplesPerMeasurement buffer to receive "amount of samples per measurement" in to
 * @param samplingTime buffer to receive "sampling time" in to
 * @param cycleTime buffer to receive "cycle time" in to
 * @return true
 */
bool CAP1188::getStandbyConfiguration(uint8_t *averageSum, uint8_t *samplesPerMeasurement, uint8_t *samplingTime, uint8_t *cycleTime)
{
    uint8_t reg;
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_STANDBY_CONFIGURATION_REG, &reg);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_STANDBY_CONFIGURATION_REG, &reg);
    }

    *averageSum = reg >> 7;
    *samplesPerMeasurement = (reg >> 4) & 0x07;
    *samplingTime = (reg >> 2) & 0x03;
    *cycleTime = reg & 0x03;

    return true;
}

/**
 * @brief Writes data to Averaging and Sampling Configuration register that controls the number of samples taken and
 * the total sensor input cycle time for all active sensor inputs while the device is functioning in Active state.
 * @param samplesPerMeasurement Determines the number of samples that are taken for all active channels during the sensor cycle (use CAP1188_SAMPLES_PER_MEASUREMENT_...)
 * @param samplingTime  Determines the time to take a single sample (use CAP1188_SAMPLING_TIME_...)
 * @param cycleTime Determines the number of samples that are taken for all active channels during the sensor cycle (use CAP1188_CYCLE_TIME_...)
 * @return true
 */
bool CAP1188::setAveragingAndSamplingConfig(uint8_t samplesPerMeasurement, uint8_t samplingTime, uint8_t cycleTime)
{
    uint8_t reg;
    reg = samplesPerMeasurement << 4 | samplingTime << 2 | cycleTime;
    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAP1188_AVERAGING_AND_SAMPLING_CONFIG_REG, reg);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAP1188_AVERAGING_AND_SAMPLING_CONFIG_REG, reg);
    }

    return true;
}

/**
 * @brief Reads data from Averaging and Sampling Configuration register that controls the number of samples taken and
 * the total sensor input cycle time for all active sensor inputs while the device is functioning in Active state.
 * @param samplesPerMeasurement buffer to receive "amount of samples per measurement" in to
 * @param samplingTime buffer to receive "sampling time" in to
 * @param cycleTime buffer to receive "cycle time" in to
 * @return true
 */
bool CAP1188::getAveragingAndSamplingConfig(uint8_t *samplesPerMeasurement, uint8_t *samplingTime, uint8_t *cycleTime)
{
    uint8_t reg;
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAP1188_AVERAGING_AND_SAMPLING_CONFIG_REG, &reg);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAP1188_AVERAGING_AND_SAMPLING_CONFIG_REG, &reg);
    }
    *samplesPerMeasurement = (reg >> 4) & 0x07;
    *samplingTime = (reg >> 2) & 0x03;
    *cycleTime = reg & 0x03;
    return true;
}
/**
 * @brief Writes data to a specific Sensor Input Threshold register, which store the delta threshold that is used to determine if a touch has been detected
 * @param buttonNumber number of a button netween 1...8
 * @param threshold threshold number (USE CAP1188_SENSOR_INPUT_THRESHOLD_...)
 * @return true on success, false on error
 */
bool CAP1188::setSensorInputThreshold(uint8_t buttonNumber, uint8_t threshold)
{
    switch (buttonNumber)
    {
    case 1:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, threshold);
        }
        break;

    case 2:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_2_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_2_THRESHOLD_REG, threshold);
        }
        break;

    case 3:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_3_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_3_THRESHOLD_REG, threshold);
        }
        break;

    case 4:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_4_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_4_THRESHOLD_REG, threshold);
        }
        break;

    case 5:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_5_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_5_THRESHOLD_REG, threshold);
        }
        break;

    case 6:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_6_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_6_THRESHOLD_REG, threshold);
        }
        break;

    case 7:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_7_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_7_THRESHOLD_REG, threshold);
        }
        break;

    case 8:
        if (!_i2c)
        {
            // SPI
            writeRegisterAtAddress(CAP1188_SENSOR_INPUT_8_THRESHOLD_REG, threshold);
        }
        else
        {
            // I2C
            I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_8_THRESHOLD_REG, threshold);
        }
        break;

    default:
        // Unknown button number received
        return false;
        break;
    }
    // TODO: Return more appropriate error code
    return true;
}
/**
 * TODO: Function does not work as expected
 * @brief Writes data to all Sensor Input Threshold registers, which store the delta threshold that is used to determine if a touch has been detected
 * @param threshold threshold number (USE CAP1188_SENSOR_INPUT_THRESHOLD_...)
 * @return 0 on success
 */
bool CAP1188::setSensorInputThresholdAll(uint8_t threshold)
{
    uint8_t reg;
    // Allow write to all SENSOR INPUT THRESHOLD REGISTERS at the same time
    // 1. Read RECALIBRATION CONFIGURATION REGISTER
    // 2. Set B7 to 0
    // 3. Write RECALIBRATION CONFIGURATION REGISTER back

    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAPP1188_RECALIBRATION_CONFIGURATION_REG, &reg);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAPP1188_RECALIBRATION_CONFIGURATION_REG, &reg);
    }

    reg &= 0x7F;

    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAPP1188_RECALIBRATION_CONFIGURATION_REG, reg);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAPP1188_RECALIBRATION_CONFIGURATION_REG, reg);
    }

    // Setting input threshold value to 1st Input register which will modify all of them
    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, threshold);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, threshold);
    }

    // Reset write to all SENSOR INPUT THRESHOLD REGISTERS at the same time (reset to default)
    // 1. Read RECALIBRATION CONFIGURATION REGISTER
    // 2. Set B7 to 1
    // 3. Write RECALIBRATION CONFIGURATION REGISTER back
    if (!_i2c)
    {
        // SPI
        readRegisterFromAddress(CAPP1188_RECALIBRATION_CONFIGURATION_REG, &reg);
    }
    else
    {
        // I2C
        I2Cdev::readByte(_i2cAddress, CAPP1188_RECALIBRATION_CONFIGURATION_REG, &reg);
    }

    reg |= 0x80;

    if (!_i2c)
    {
        // SPI
        writeRegisterAtAddress(CAPP1188_RECALIBRATION_CONFIGURATION_REG, reg);
    }
    else
    {
        // I2C
        I2Cdev::writeByte(_i2cAddress, CAPP1188_RECALIBRATION_CONFIGURATION_REG, reg);
    }

    return 0;
}

/**
 * @brief Reads data from Sensor Input Threshold registers, which store the delta threshold that is used to determine if a touch has been detected
 * @param buttonNumber number of a button netween 1...8
 * @return register content with selected button number threshold, 0 on error
 */
uint8_t CAP1188::getSensorInputThreshold(uint8_t buttonNumber)
{
    uint8_t reg;
    switch (buttonNumber)
    {
    case 1:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_1_THRESHOLD_REG, &reg);
        }
        break;

    case 2:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_2_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_2_THRESHOLD_REG, &reg);
        }
        break;

    case 3:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_3_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_3_THRESHOLD_REG, &reg);
        }
        break;

    case 4:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_4_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_4_THRESHOLD_REG, &reg);
        }
        break;

    case 5:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_5_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_5_THRESHOLD_REG, &reg);
        }
        break;

    case 6:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_6_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_6_THRESHOLD_REG, &reg);
        }
        break;

    case 7:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_7_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_7_THRESHOLD_REG, &reg);
        }
        break;

    case 8:
        if (!_i2c)
        {
            // SPI
            readRegisterFromAddress(CAP1188_SENSOR_INPUT_8_THRESHOLD_REG, &reg);
        }
        else
        {
            // I2C
            I2Cdev::readByte(_i2cAddress, CAP1188_SENSOR_INPUT_8_THRESHOLD_REG, &reg);
        }
        break;

    default:
        // Unknown button received
        return 0;
        break;
    }

    return reg;
}

/*  Below is minimal SPI functionality set required to communicate with CAP1188 via SPI */

/**
 * @brief Sends a command to reset SPI interface on CAP1188
 * @return true
 */
bool CAP1188::resetSPI()
{
    // When 0x7A is sent twice, SPI communication interface is reset on CAP1188
    uint8_t buff[2] = {0x7A, 0x7A};
    digitalWrite(_csPin, LOW); // Asserting CS pin low to begin communication
    SPI.transferBytes(buff, NULL, 2);
    digitalWrite(_csPin, HIGH); // Asserting CS pin high to end communication
    return true;
}

/**
 * @brief Sends a command via SPI to set register pointer for CAP1188
 * @param regAddress regiuster address in hex
 * @return true
 */
bool CAP1188::setRegisterAddress(uint8_t regAddress)
{
    // When 0x7D followed by REGISTER ADDRESS is sent,
    // register pointer on CAP1188 is set to a desired
    // REGISTER ADDRESS to read/write data from/to
    uint8_t buff[2] = {0x7D, regAddress};
    digitalWrite(_csPin, LOW); // Asserting CS pin low to begin communication
    SPI.transferBytes(buff, NULL, 2);
    digitalWrite(_csPin, HIGH); // Asserting CS pin high to end communication
    return true;
}

/**
 * @brief Sends a command via SPI to read register currently pointed by register pointer from CAP1188.
 * NOTE: Register pointer needs to be set before calling this function to read data from a desired register
 * @return Register data
 */
uint8_t CAP1188::readRegister()
{
    // When 0x7F is sent, CAP1188 sends data from
    // a register, pointed by register pointer by CAP1188
    uint8_t buff = 0x7F;
    digitalWrite(_csPin, LOW); // Asserting CS pin low to begin communication
    SPI.transferBytes(&buff, NULL, 1);
    SPI.transferBytes(NULL, &buff, 1); // Reusing variable to store contents of a read register
    digitalWrite(_csPin, HIGH);        // Asserting CS pin high to end communication
    return buff;
}

/**
 * @brief Sends a command via SPI to write data to a register currently pointed by register pointer from CAP1188.
 * NOTE: Register pointer needs to be set before calling this function to write data to a desired register
 * @return true
 */
bool CAP1188::writeRegister(uint8_t data)
{
    // When 0x7E followed by DATA is sent,
    // data is written to a register, pointed
    // by a register pointer by CAP1188
    uint8_t buff[2] = {0x7E, data};
    digitalWrite(_csPin, LOW); // Asserting CS pin low to begin communication
    SPI.transferBytes(buff, NULL, 2);
    digitalWrite(_csPin, HIGH); // Asserting CS pin high to end communication
    return true;
}

/**
 * @brief Sends commands via SPI to CAP1188 to: (1) set a register pointer (2) write data to a pointed register
 * @param regAddress register address to write to
 * @param data data to write into regiser
 * @return true
 */
bool CAP1188::writeRegisterAtAddress(uint8_t regAddress, uint8_t data)
{
    setRegisterAddress(regAddress);
    writeRegister(data);
    return true;
}

/**
 * @brief Sends commands via SPI to CAP1188 to: (1) set a register pointer (2) read data from a register, pointed by register pointer
 * @param regAddress register address to read from
 * @param data pointer to a buffer data to be read in
 * @return void
 */
void CAP1188::readRegisterFromAddress(uint8_t regAddress, uint8_t* data)
{
    setRegisterAddress(regAddress);
    *data = readRegister();
}
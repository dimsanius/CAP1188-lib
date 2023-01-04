// Selects CAP1188 Address. Use resistors, connected to PIN 14 in pull-down mode (pin 14 -> GND) to adjut its address:
//  1. GND:                 SPI Communications using Normal 4-wire Protocol
//  2. 56k to GND:          SPI Communications using BiDirectional 3-wire Protocol
//  3. 68k to GND:          Reserved
//  4. VDD:                 I2C address 0x28
//  5. 150k to GND:         I2C address 0x29
//  6. 120k to GND:         I2C address 0x2A
//  7. 100k to GND:         I2C address 0x2B
//  8. 82k to GND:          I2C address 0x2C
#define CAP1188_I2C_ADDRESS_VDD             0x28
#define CAP1188_I2C_ADDRESS_82K             0x29
#define CAP1188_I2C_ADDRESS_100K            0x2A
#define CAP1188_I2C_ADDRESS_120K            0x2B
#define CAP1188_I2C_ADDRESS_150K            0x2C
// Most CAP1188 dev boards use default address of 0x29 (150k to GND). Feel free to adjust the default address.
#define CAP1188_I2C_DEFAULT_ADDRESS         CAP1188_I2C_ADDRESS_82K

// Various options

// Used for STANDBY CONFIGURATION REGISTER (0x41), B7
#define CAP1188_AVG_SUM_AVG                              0x00
#define CAP1188_AVG_SUM_SUM                              0x01

// Used for STANDBY CONFIGURATION REGISTER (0x41), B6-B4
#define CAP1188_SAMPLES_PER_MEASUREMENT_1                0x00
#define CAP1188_SAMPLES_PER_MEASUREMENT_2                0x01
#define CAP1188_SAMPLES_PER_MEASUREMENT_4                0x02
#define CAP1188_SAMPLES_PER_MEASUREMENT_8                0x03
#define CAP1188_SAMPLES_PER_MEASUREMENT_16               0x04
#define CAP1188_SAMPLES_PER_MEASUREMENT_32               0x05
#define CAP1188_SAMPLES_PER_MEASUREMENT_64               0x06
#define CAP1188_SAMPLES_PER_MEASUREMENT_128              0x07

// Used for STANDBY CONFIGURATION REGISTER (0x41), B3-B2
#define CAP1188_SAMPLING_TIME_320us                      0x00
#define CAP1188_SAMPLING_TIME_640us                      0x01
#define CAP1188_SAMPLING_TIME_1_28ms                     0x02
#define CAP1188_SAMPLING_TIME_2_56ms                     0x03

// Used for STANDBY CONFIGURATION REGISTER (0x41), B1-B0
#define CAP1188_CYCLE_TIME_35ms                          0x00
#define CAP1188_CYCLE_TIME_70ms                          0x01
#define CAP1188_CYCLE_TIME_105ms                         0x02
#define CAP1188_CYCLE_TIME_140ms                         0x03

// Used for SENSOR INPUT THRESHOLD REGISTERS(0x30-0x37)
#define CAP1188_SENSOR_INPUT_THRESHOLD_1                 0x01
#define CAP1188_SENSOR_INPUT_THRESHOLD_2                 0x02
#define CAP1188_SENSOR_INPUT_THRESHOLD_4                 0x04
#define CAP1188_SENSOR_INPUT_THRESHOLD_8                 0x08
#define CAP1188_SENSOR_INPUT_THRESHOLD_16                0x10
#define CAP1188_SENSOR_INPUT_THRESHOLD_32                0x20
#define CAP1188_SENSOR_INPUT_THRESHOLD_64                0x40

class CAP1188{
    public:
        CAP1188();
        void initI2C(uint8_t address, uint8_t resetPin);
        void initSPI(uint8_t csPin, uint8_t resetPin);
        uint8_t getProductId();
        uint8_t getManufacturerId();
        uint8_t getRevision();
        uint8_t getSensorInputs();
        bool setMultipleTouchConfiguration(bool multipleTouchCircuitryEnable, uint8_t numberOfSimultaneousTouches);
        bool setSensorInputLEDLinking(uint8_t ledsToLink);
        bool setStandbyConfiguration(uint8_t averageSum, uint8_t samplesPerMeasurement, uint8_t samplingTime, uint8_t cycleTime);
        bool getStandbyConfiguration(uint8_t* averageSum, uint8_t *samplesPerMeasurement, uint8_t *samplingTime, uint8_t *cycleTime);
        bool setAveragingAndSamplingConfig(uint8_t samplesPerMeasurement, uint8_t samplingTime, uint8_t cycleTime);
        bool getAveragingAndSamplingConfig(uint8_t *samplesPerMeasurement, uint8_t *samplingTime, uint8_t *cycleTime);
        bool setSensorInputThershold(uint8_t inputNumber, uint8_t threshold);
        bool setSensorInputThersholdAll(uint8_t threshold);
        uint8_t getSensorInputThershold(uint8_t inputNumber);
        uint8_t readRegisterFromAddress(uint8_t regAddress);
        bool writeRegisterAtAddress(uint8_t regAddress, uint8_t data);
    private:
        bool resetSPI();
        bool setRegisterAddress(uint8_t regAddress);
        uint8_t readRegister();
        bool writeRegister(uint8_t data);
        bool _i2c;
        int8_t _i2cAddress, _resetPin, _csPin;
};
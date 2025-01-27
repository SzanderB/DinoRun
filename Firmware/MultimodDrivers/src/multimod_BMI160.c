// multimod_BMI160.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for BMI160 functions

/************************************Includes***************************************/

#include "../multimod_BMI160.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/
#define delay_0_1_s     1600000
#define ACCEL
//#define SELFTESTGYRO
//#define GYRO
//#define MAG
/********************************Public Functions***********************************/

// BMI160_Init
// Initializes the BMI160. Currently enables the accelerometer
// in full-power mode.
// Return: void
void BMI160_Init() {
    I2C_Init(I2C_A_BASE);
    // Power on accelerometer


    // throw a reset on that lil thing
    BMI160_WriteRegister(BMI160_CMD_ADDR, BMI160_SOFTRESET);
//     wait while it resets
    SysCtlDelay(1600000);


#ifdef GYRO
    // Set power
    BMI160_WriteRegister(BMI160_CMD_ADDR, BMI160_GYROPMUNORMAL);
    SysCtlDelay(900000);    //delay ~55 ms
    //Set ACC Config regs
    BMI160_WriteRegister(BMI160_ACCCONF_ADDR, BMI160_GYROCONFIG);
    BMI160_WriteRegister(BMI160_ACCRANGE_ADDR, BMI160_GYRORANGE);



#endif


#ifdef SELFTESTGYRO
    BMI160_WriteRegister(BMI160_SELFTEST, 0x10);
    SysCtlDelay(1600000/5);
    uint8_t testval = 0;
    while(true){

        testval = BMI160_ReadRegister(BMI160_STATUS_ADDR);

    }


#endif

#ifdef ACCEL

    //Set ACC Config regs


    BMI160_WriteRegister(BMI160_CMD_ADDR, BMI160_ACCPMUNORMAL);
    SysCtlDelay(160000);

    BMI160_WriteRegister(BMI160_ACCCONF_ADDR, BMI160_ACCDATARATE);
    BMI160_WriteRegister(BMI160_ACCRANGE_ADDR, BMI160_ACCRANGE);

#endif


#ifdef  MAG
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x19);
    SysCtlDelay(1600000);

//    BMI160_WriteRegister(BMI160_MAGCONF_ADDR, 0x08);
    // base address is 0x13, but there is an extra 0 at the left
    BMI160_WriteRegister(0x4B, (0x13<<1));           // write to MAG_IF[0] to set base i2c address

    BMI160_WriteRegister(0x4C, 0x81);       // write MAG[1] to enable programming

    // configure secondary interface
    BMI160_WriteRegister(0x6B, 0b00100000);     // enable IF register

    // write 1 to power mode on BMM
    BMI160_WriteRegister(0x4F, 0x01);
    BMI160_WriteRegister(0x4E, 0x4B);

        // Turn ODR for BMM to 25hz
    BMI160_WriteRegister(0x4F, 0x30);
    BMI160_WriteRegister(0x4E, 0x4C);

    // set repetitions z
    BMI160_WriteRegister(0x4F, 0x04);
    BMI160_WriteRegister(0x4E, 0x51);

        // set repetitions for z
    BMI160_WriteRegister(0x4F, 0x04);
    BMI160_WriteRegister(0x4E, 0x52);

    BMI160_WriteRegister(0x4D, 0x42);       // set read reg to data reg of BMM

    BMI160_WriteRegister(BMI160_MAGCONF_ADDR, 0x06);    // set it to 25Hz ODR

    // turn off manual read
    BMI160_WriteRegister(0x4C, 0x01);
    pmu_status = BMI160_ReadRegister(0x03);
    SysCtlDelay(1600000);
    pmu_status = BMI160_ReadRegister(0x03);
#endif

    return;
}

// BMI160_Init
// Writes to a register address in the BMI160.
// Param uint8_t "addr": Register address
// Param uint8_t "data": data to write
// Return: void
void BMI160_WriteRegister(uint8_t addr, uint8_t data) {
    // Complete this function

    uint8_t arr[2] = {addr, data};

    I2C_WriteMultiple(I2C_A_BASE, BMI160_ADDR, arr, 2);

    return;
}

// BMI160_ReadRegister
// Reads from a register address in the BMI160.
// Param uint8_t "addr": Register address
// Return: void
uint8_t BMI160_ReadRegister(uint8_t addr) {
    // Complete this function
    // send single read command, but make sure a NACK is sent

    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);

    I2CSlaveACKOverride(I2C_A_BASE, true);
    I2CSlaveACKValueSet(I2C_A_BASE, false);

    return I2C_ReadSingle(I2C_A_BASE, BMI160_ADDR);
}

// BMI160_MultiReadRegister
// Uses the BMI160 auto-increment function to read from multiple registers.
// Param uint8_t "addr": beginning register address
// Param uint8_t* "data": pointer to an array to store data in
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void BMI160_MultiReadRegister(uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // Complete this function
    // same idea as above, but only change the I2CSlaveACK at the end
    I2C_WriteSingle(I2C_A_BASE, BMI160_ADDR, addr);


        // Set the address in the slave address register
    I2CMasterSlaveAddrSet(I2C_A_BASE, BMI160_ADDR, true);
    // Trigger I2C module receive
    I2CMasterControl(I2C_A_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(I2C_A_BASE))
    {
    }
    // Read received data
    *data = I2CMasterDataGet(I2C_A_BASE);

    data += 1;
    num_bytes-= 1;


    while(num_bytes > 1){
    // While num_bytes > 1
        // Trigger I2C module receive
        I2CMasterControl(I2C_A_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        // Wait until I2C module is no longer busy
        while(I2CMasterBusy(I2C_A_BASE))
        {
        }
        // Read received data
        *data = I2CMasterDataGet(I2C_A_BASE);
        data += 1;
        num_bytes -= 1;
    }

    I2CSlaveACKOverride(I2C_A_BASE, true);
    I2CSlaveACKValueSet(I2C_A_BASE, false);
    // Trigger I2C module receive
    I2CMasterControl(I2C_A_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(I2C_A_BASE))
    {
    }
    // Read last byte
    *data = I2CMasterDataGet(I2C_A_BASE);

    return;

}

// BMI160_AccelXGetResult
// Gets the 16-bit x-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelXGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));
    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelYGetResult
// Gets the 16-bit y-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelYGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelZGetResult
// Gets the 16-bit z-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelZGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroXGetResult
// Gets the 16-bit x-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroXGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroYGetResult
// Gets the 16-bit y-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroYGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroZGetResult
// Gets the 16-bit z-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroZGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagXGetResult
// Gets the 16-bit x-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagXGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagYGetResult
// Gets the 16-bit y-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagYGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagZGetResult
// Gets the 16-bit z-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagZGetResult() {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG));

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelXYZGetResult
// Stores the 16-bit XYZ accelerometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_AccelXYZGetResult(uint16_t* data) {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC));

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GyroXYZGetResult
// Stores the 16-bit XYZ gyroscope results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_GyroXYZGetResult(uint16_t* data) {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR));

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_MagXYZGetResult
// Stores the 16-bit XYZ magnetometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_MagXYZGetResult(uint16_t* data) {
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG));

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GetDataStatus
// Gets the status register to determine if data is ready to read.
// Return: uint8_t
uint8_t BMI160_GetDataStatus() {
    return BMI160_ReadRegister(BMI160_STATUS_ADDR);
}

/********************************Public Functions***********************************/

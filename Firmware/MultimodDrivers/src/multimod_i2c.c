// multimod_i2c.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for I2C functions

/************************************Includes***************************************/

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// I2C_Init
// Initializes specified I2C module
// Param uint32_t "mod": base address of module
// Return: void
void I2C_Init(uint32_t mod) {
    // Note:    The multimod board uses multiple I2C modules
//              to communicate with different devices. You can use
    //          the 'mod' parameter to choose which module to initialize
    //          and use.

    // Enable clock to relevant I2C and GPIO modules
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    // Configure pins for I2C module
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(I2C_A_GPIO_BASE, I2C_A_PIN_SCL);
    GPIOPinTypeI2C(I2C_A_GPIO_BASE, I2C_A_PIN_SDA);

    // Configure I2C SCL speed, set as master
    I2CMasterInitExpClk(mod, SysCtlClockGet(), false);

    return;
}

// I2C_WriteSingle
// Writes a single byte to an address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t "byte": byte to send
// Return: void
void I2C_WriteSingle(uint32_t mod, uint8_t addr, uint8_t byte) {
    // Set the address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, false);
    // Input data into I2C module
    I2CMasterDataPut(mod, byte);
    // Trigger I2C module send
    I2CMasterControl(mod, I2C_MASTER_CMD_SINGLE_SEND);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod))
    {
    }

    return;
}

// I2C_ReadSingle
// Reads a single byte from address.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Return: uint8_t
uint8_t I2C_ReadSingle(uint32_t mod, uint8_t addr) {
    // Set the address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, true);
    // Trigger I2C module receive
    I2CMasterControl(mod, I2C_MASTER_CMD_SINGLE_RECEIVE);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod))
    {
    }
    // Return received data
    return I2CMasterDataGet(mod);
}

// I2C_WriteMultiple
// Write multiple bytes to a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to transmit
// Return: void
void I2C_WriteMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // Set the address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, false);
    // Input data into I2C module
    I2CMasterDataPut(mod, *data);
    // Trigger I2C module send
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod))
    {
    }
    num_bytes -=1;
    data +=1;

    if(num_bytes == 1){
        I2CMasterDataPut(mod, *(data));
        I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(mod))
        {
        }

        return;
    }else{
        while(num_bytes > 1){
        // While num_bytes > 1
            // Input data into I2C module
            I2CMasterDataPut(mod, *(data));
            // Trigger I2C module send
            I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_CONT);
            // Wait until I2C module is no longer busy
            while(I2CMasterBusy(mod))
            {
            }
            num_bytes -=1;
            data +=1;
        }
        // Input last byte into I2C module
        I2CMasterDataPut(mod, *(data));
        // Trigger I2C module send

        I2CMasterControl(mod, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(mod))
        {
        }
    }


    return;
}

// I2C_ReadMultiple
// Read multiple bytes from a device.
// Param uint32_t "mod": base address of module
// Param uint8_t "addr": address to device
// Param uint8_t* "data": pointer to an array of bytes
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void I2C_ReadMultiple(uint32_t mod, uint8_t addr, uint8_t* data, uint8_t num_bytes) {
    // Set the address in the slave address register
    I2CMasterSlaveAddrSet(mod, addr, true);
    // Trigger I2C module receive
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_START);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod))
    {
    }
    // Read received data
    *data = I2CMasterDataGet(mod);

    data += 1;
    num_bytes -= 1;

    while(num_bytes > 1){
    // While num_bytes > 1
        // Trigger I2C module receive
        I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        // Wait until I2C module is no longer busy
        while(I2CMasterBusy(mod))
        {
        }
        // Read received data
        *data = I2CMasterDataGet(mod);
        data += 1;
        num_bytes -= 1;
    }
    // Trigger I2C module receive
    I2CMasterControl(mod, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    // Wait until I2C module is no longer busy
    while(I2CMasterBusy(mod))
    {
    }
    // Read last byte
    *data = I2CMasterDataGet(mod);

    return;
}

/********************************Public Functions***********************************/


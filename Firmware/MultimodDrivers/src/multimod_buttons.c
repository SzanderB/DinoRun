// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_buttons.h"

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init() {
    // Initialize this function & the relevant interrupt pin
    //PE4 pin connects to PCA
        // Enable clock to port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // Wait for it to turn out
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }
    // set to input
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    // Active low signal, so trigger on falling edge
    GPIOIntTypeSet(GPIO_PORTE_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE);
    // enable interrupts
    GPIOIntEnable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t 
uint8_t MultimodButtons_Get() {
    // complete this function
    // Write the command 0 to the PCA first
    I2C_WriteSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR, 0);

    I2CSlaveACKOverride(I2C_A_BASE, true);
    I2CSlaveACKValueSet(I2C_A_BASE, false);
    //then read a single byte, but make sure to NACK after
    uint8_t data = I2C_ReadSingle(I2C_A_BASE, BUTTONS_PCA9555_GPIO_ADDR);

    return ~(data);
}


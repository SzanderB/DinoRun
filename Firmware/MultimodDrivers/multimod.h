#ifndef MULTIMOD_H_
#define MULTIMOD_H_

#include "multimod_BMI160.h"
#include "multimod_i2c.h"
#include "multimod_joystick.h"
#include "multimod_OPT3001.h"
#include "multimod_PCA9956b.h"
#include "multimod_uart.h"
#include "multimod_ST7789.h"
#include "multimod_spi.h"
#include "multimod_buttons.h"

//For testing
#include "multimod_LaunchpadButtons.h"
#include "multimod_LaunchpadLED.h"

static void multimod_init() {
   UART_Init();
   ST7789_Init();
//    BMI160_Init();
//    OPT3001_Init();
   PCA9956b_Init();
   

   MultimodButtons_Init();
   JOYSTICK_Init();
   JOYSTICK_IntEnable();

    // UART_Init();
    // BMI160_Init();
    // OPT3001_Init();
    // LaunchpadButtons_Init();
    // LaunchpadLED_Init();
}

#endif /* MULTIMOD_H_ */

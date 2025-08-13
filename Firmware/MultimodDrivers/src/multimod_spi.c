...

// SPI_Init
// Initializes specified SPI module. By default the mode
// is set to communicate with the TFT display.
// Param uint32_t "mod": base address of module
// Return: void
void SPI_Init(uint32_t mod) {

    if (mod == SPI_A_BASE) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

        GPIOPinConfigure(GPIO_PA2_SSI0CLK);
        // GPIOPinConfigure(GPIO_PA3_SSI0FSS);
        GPIOPinConfigure(GPIO_PA4_SSI0RX);
        GPIOPinConfigure(GPIO_PA5_SSI0TX);

        GPIOPinTypeSSI(SPI_A_GPIO_BASE, SPI_A_PIN_MISO | SPI_A_PIN_MOSI | SPI_A_PIN_CLK);

        SSIDisable(mod);
        SSIClockSourceSet(mod, SSI_CLOCK_SYSTEM);
        SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 15000000, 8);
        SSIEnable(mod);
    }
}

...

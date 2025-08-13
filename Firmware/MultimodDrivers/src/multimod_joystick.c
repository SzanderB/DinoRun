...


// JOYSTICK_GetXY
// Gets X and Y adc readings
// Return: uint32_t, 16-bit packed, upper 16-bit is X and lower 16-bit is Y.
uint32_t JOYSTICK_GetXY() {
    uint32_t results[2];

    // Start conversion
    ADCProcessorTrigger(ADC0_BASE, 2);

    // Wait until conversion is complete
    while(!ADCIntStatus(ADC0_BASE, 2, 0));

    // Clear ADC interrupt flag
    ADCIntClear(ADC0_BASE, 2);

    // Read from x and y coordinates
    ADCSequenceDataGet(ADC0_BASE, 2, results);

    return (results[0] << 16 | results[1]);
}



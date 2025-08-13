...
    
// ST7789_SetWindow
// Sets windows subsequent pixels will be generated at.
// Param int16_t x: x-coord of first corner.
// Param int16_t y: y-coord of first corner.
// Param int16_t w: width of window.
// Param int16_t h: height of window.
// Return: void
void ST7789_SetWindow(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Your code here!

    // Check boundary conditions
    if(x < 0){
        x=0;
    }
    if(x>X_MAX){
        x = X_MAX-1-50;
    }
    if(y < 0){
        y=0;
    }
    if(y>Y_MAX){
        y = Y_MAX-1-50;
    }


    // Set column address

    ST7789_WriteCommand(ST7789_CASET_ADDR);
    ST7789_WriteData((uint8_t)(x>>8));
    ST7789_WriteData((uint8_t)(x & 0xFF));
    ST7789_WriteData((uint8_t)((x+w-1)>>8));
    ST7789_WriteData((uint8_t)((x+w-1) & 0xFF));
    
    // Set row address
    ST7789_WriteCommand(ST7789_RASET_ADDR);
    ST7789_WriteData((uint8_t)(y>>8));
    ST7789_WriteData((uint8_t)(y & 0xFF));
    ST7789_WriteData((uint8_t)((y+h-1)>>8));
    ST7789_WriteData((uint8_t)((y+h-1) & 0xFF));

    // Set register to write to as memory
    ST7789_WriteCommand(ST7789_RAMWR_ADDR);
}

...

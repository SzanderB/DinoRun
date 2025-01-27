// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h>




/*********************************Global Variables**********************************/
bool gameOver= false;
bool jumping = false;
bool crouch = false;
int dinoY = 26;
uint8_t numBlocks = 0;


/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void) {
    while(1);
}

void Dino_Thread(void) {
    // Initialize / declare any variables here
    int16_t button;
    int16_t joystick;
    // draw the floor
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawHLine(0, 25, 240, ST7789_BLUE);
    //draw dino
    ST7789_DrawRectangle(35, 26, 10, 4, ST7789_WHITE);
    G8RTOS_SignalSemaphore(&sem_SPIA);
    while(1) {
        if(gameOver){
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_Fill(ST7789_RED);
            G8RTOS_SignalSemaphore(&sem_SPIA);
            sleep(2000);
            continue;
        }
        joystick = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
        if(joystick == -2){
            sleep(50);
            continue;
        }else if(joystick >= MIN_JOYSTICKVAL){
            jumping = true;
            // drawing 6 frames going up, 6 frames going down
            G8RTOS_WaitSemaphore(&sem_SPIA);
            int i = 0;
            while(i < 6){
                G8RTOS_SignalSemaphore(&sem_SPIA);
                sleep(75);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(35, dinoY, 10, 4, ST7789_BLACK);
                i++;
                dinoY = 26 + i*10;
                ST7789_DrawRectangle(35, dinoY, 10, 4, ST7789_WHITE);
            }
            i = 0;
            while(i < 6){
                G8RTOS_SignalSemaphore(&sem_SPIA);
                sleep(75);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(35, dinoY, 10, 4, ST7789_BLACK);
                i++;
                dinoY = 86 -i*10;
                ST7789_DrawRectangle(35, dinoY, 10, 4, ST7789_WHITE);
            }
            G8RTOS_SignalSemaphore(&sem_SPIA);
            jumping = false;
        }
        // sleep
        sleep(50);
    }
}

void Blocks_Thread(){
    //initialize randomness
    time_t t;
    srand((unsigned) time(&t));
    
    uint16_t randomTime;
    while(1){
        if(gameOver){
            sleep(2000);
            continue;
        }else if(numBlocks > 3){        // determines whether a new block needs to be sent
            sleep(50);
            continue;
        }
        randomTime =(rand() % 1001) + 1400;

        G8RTOS_AddThread(jumpBlock, 252, "jump\0");
        numBlocks++;

        sleep(randomTime);
    }

}

void jumpBlock(void){
    int currX = 240;
    while(1){
        if(gameOver){
            numBlocks--;
            G8RTOS_KillSelf();
        }
        // draw a block at current x
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(currX, 26, 10, 2, ST7789_BLACK);
        currX -=6;
        if(currX < 0){
            numBlocks--;
            G8RTOS_SignalSemaphore(&sem_SPIA);
            G8RTOS_KillSelf();
        }
        ST7789_DrawRectangle(currX, 26, 10, 2, ST7789_GREEN);
        G8RTOS_SignalSemaphore(&sem_SPIA);
        //check if dino was jumping, if not, kill it
        if(currX >= 20 && currX <= 41){
            if(!jumping){
                G8RTOS_WaitSemaphore(&sem_UART);
                UARTprintf("You lose!");
                G8RTOS_SignalSemaphore(&sem_UART);
                gameOver = true;
                G8RTOS_KillSelf();
            }
        }
        sleep(125);
    }
}

void Read_Buttons() {
    // Initialize / declare any variables here
    uint8_t buttons;
    uint8_t button1 = 0x2;

    while(1) {
        // Wait for a signal to read the buttons on the Multimod board.

        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        // Sleep to debounce
        sleep(10);

        // Read the buttons status on the Multimod board.
        G8RTOS_WaitSemaphore(&sem_I2CA);
        buttons = MultimodButtons_Get();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        // Process the buttons and determine what actions need to be performed.
        
        if((buttons & button1) == 0x2){                   //button 1 is JUMP
            gameOver = false;
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_Fill(ST7789_BLACK);
            // draw the floor
            ST7789_DrawHLine(0, 25, 240, ST7789_BLUE);
            //draw dino
            ST7789_DrawRectangle(35, 26, 10, 4, ST7789_WHITE);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
        // Clear the interrupt
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_4);
        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);
    }
}

//void Read_JoystickPress() {
//    // Initialize / declare any variables here
//
//    while(1) {
//        // Wait for a signal to read the joystick press
//
//        G8RTOS_WaitSemaphore(&sem_Joystick_Debounce);
//        // Sleep to debounce
//        sleep(10);
//        // Read the joystick switch status on the Multimod board.
//        if(JOYSTICK_GetPress()){
//
//            // Toggle the joystick_y flag.
//            joystick_y = !(joystick_y);
//
//        }
//        // Clear the interrupt
//        GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
//        // Re-enable the interrupt so it can occur again.
//        GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
//    }
//}



/********************************Periodic Threads***********************************/

//void Print_WorldCoords(void) {
//    // Print the camera position through UART to display on console.
//    G8RTOS_WaitSemaphore(&sem_UART);
//    UARTprintf("X: %d, Y: %d, Z: %d\n", (int)(world_camera_pos.x), (int)(world_camera_pos.y), (int)(world_camera_pos.z));
//    G8RTOS_SignalSemaphore(&sem_UART);
//}

void Get_Joystick(void) {
    // Read the joystick
    if(jumping){
        return;
    }
    if(JOYSTICK_GetX() > 4000){
        if(getSize(JOYSTICK_FIFO) == 0){
            G8RTOS_WriteFIFO(JOYSTICK_FIFO, JOYSTICK_GetX());
        }
    }
    // Send through FIFO.
}



/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler() {
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);
    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

//void GPIOD_Handler() {
//    // Disable interrupt
//    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
//    // Signal relevant semaphore
//    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
//}

// Lab 4, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 4 is intended to introduce you to more advanced RTOS concepts. In this, you will
// - implement blocking, yielding, sleeping
// - Thread priorities, aperiodic & periodic threads
// - IPC using FIFOs
// - Dynamic thread creation & deletion

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"


/************************************MAIN*******************************************/
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, initialize semaphores here!
    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_UART, 1);
//    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 1);
//    G8RTOS_InitSemaphore(&sem_KillCube,1);

    G8RTOS_AddThread(Idle_Thread, 254, "idle\0");
    G8RTOS_AddThread(Dino_Thread, 253, "dino\0");
    G8RTOS_AddThread(Read_Buttons, 252, "buttons\0");
    G8RTOS_AddThread(Blocks_Thread, 254, "blocks\0");
//
   G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 5, 20);
//    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 5, 19);

    // G8RTOS_Add_PeriodicEvent(Print_WorldCoords, 1000, 2);
    G8RTOS_Add_PeriodicEvent(Get_Joystick, 50, 1);

   G8RTOS_InitFIFO(JOYSTICK_FIFO);
    G8RTOS_InitFIFO(JUMP_FIFO);


    G8RTOS_Launch();
    while (1);
}

/************************************MAIN*******************************************/

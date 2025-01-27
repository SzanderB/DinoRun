// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define JUMP_FIFO      0
#define JOYSTICK_FIFO       1

#define MIN_JOYSTICKVAL     4000

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_KillCube;

//used for testing only
semaphore_t sem_UART;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/


/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Dino_Thread(void);
void Read_Buttons(void);
void Blocks_Thread(void);
void jumpBlock(void);
void crouchBlock(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Print_WorldCoords(void);
void Get_Joystick(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler(void);
void GPIOD_Handler(void);

/*******************************Aperiodic Threads***********************************/


/*******************************Testing Threads***********************************/
void Thread0(void);
void Thread0EC(void);
void Thread1(void);
void Thread2(void);
void Thread3(void);
void Thread4(void);

/*******************************Testing Threads***********************************/


#endif /* THREADS_H_ */


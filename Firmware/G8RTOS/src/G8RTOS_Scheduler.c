
// G8RTOS_Scheduler
// Chooses next thread in the TCB. This time uses priority scheduling.
// Return: void
void G8RTOS_Scheduler() {
    // Using priority, determine the most eligible thread to run that
    // is not blocked or asleep. Set current thread to this thread's TCB.
    tcb_t* selected = CurrentlyRunningThread;
    // CurrentlyRunningThread = CurrentlyRunningThread->nextTCB;
    tcb_t *pt = CurrentlyRunningThread->nextTCB;
    uint16_t maxPrio = UINT8_MAX + 1;

    for(int i = 0; i < NumberOfThreads-1; i++){
        if(pt->asleep || (pt->blocked != NULL)){
            pt = pt->nextTCB;
            continue;
        }else if (pt->priority < maxPrio)
        {
            selected = pt;
            maxPrio = pt->priority;
        }
        pt = pt->nextTCB;
    }
    CurrentlyRunningThread = selected;
}

// G8RTOS_AddThread
// Adds a thread. This is now in a critical section to support dynamic threads.
// It also now should initalize priority and account for live or dead threads.
// Param void* "threadToAdd": pointer to thread function address
// Param uint8_t "priority": priority from 0, 255.
// Param char* "name": character array containing the thread name.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *name) {
    IBit_State = StartCriticalSection();
    // If number of threads is greater than the maximum number of threads
    if(NumberOfThreads >= MAX_THREADS){
        // return
        EndCriticalSection(IBit_State);
        return THREAD_LIMIT_REACHED;
    } else{
    // else
        // if no threads
        // This should only trigger once, since there will never be allowed to have 0 active threads again
        if(NumberOfThreads == 0){
            // Next and Previous TCBs will be itself
            threadControlBlocks[0].previousTCB = &threadControlBlocks[0];
            threadControlBlocks[0].nextTCB = &threadControlBlocks[0];
            // Init the stack
            SetInitialStack(0);
            // Set the PC to the function desired
            threadStacks[0][STACKSIZE-2] = (uint32_t)(threadToAdd);

            // add name and priority
            strncpy(threadControlBlocks[0].threadName, name, sizeof(threadControlBlocks[0].threadName) - 1);
            threadControlBlocks[0].threadName[sizeof(threadControlBlocks[0].threadName) - 1] = '\0'; // Ensure null-termination
            threadControlBlocks[0].priority = priority;
            threadControlBlocks[0].isAlive = true;

            threadControlBlocks[0].ThreadID = 0;
            
            // Increment #threads
            NumberOfThreads += 1;
            threadCounter++;
        }else{
            for(int i = 0; i < NumberOfThreads; i++){
                // check to see if there are any dead threads in the middle of the pack, if so replace it
                if(threadControlBlocks[i].isAlive == false){
                    //update the name, priority, and PC associated with it
                    threadStacks[i][STACKSIZE-2] = (uint32_t)(threadToAdd);
                    threadControlBlocks[i].priority = priority;

                    strncpy(threadControlBlocks[NumberOfThreads].threadName, name, sizeof(threadControlBlocks[NumberOfThreads].threadName) - 1);
                    threadControlBlocks[NumberOfThreads].threadName[sizeof(threadControlBlocks[NumberOfThreads].threadName) - 1] = '\0'; // Ensure null-termination
                    
                    //update the next and previous threads of the current thread AND update the next thread of the previous one, and the previous thread of the next one
                        // need to loop through to find the first one that isnt dead
                    for(int j = i+1; j < threadCounter; j++){
                        if(threadControlBlocks[j].isAlive){
                            threadControlBlocks[i].nextTCB = &threadControlBlocks[j];
                            threadControlBlocks[j].previousTCB = &threadControlBlocks[i];
                            break;
                        }
                    }

                    //change the previous TCB
                    for(int j = i-1; j >= 0; j--){
                        if(threadControlBlocks[j].isAlive){
                            threadControlBlocks[i].previousTCB = &threadControlBlocks[j];
                            threadControlBlocks[j].nextTCB = &threadControlBlocks[i];
                            break;
                        }
                    }
                    //BUT if there are no alive threads after it, it needs to loop until it finds the next one, starting at the front of the list
                    // this should cover all possibilities, like if you have 6 dead threads, and 2 live ones (after this function), and you place the new live thread after the only other live thread
                    // Should only trigger if the first thread is still alive, and all the rest are dead
                    if(threadControlBlocks[i].previousTCB == NULL){
                        for(int j = threadCounter-1; j > i; j--){
                            if(threadControlBlocks[j].isAlive){
                                threadControlBlocks[i].previousTCB = &threadControlBlocks[j];
                                threadControlBlocks[j].nextTCB = &threadControlBlocks[i];
                                break;
                            }
                        }
                    }
                    
                    // make sure it is marked alive again
                    threadControlBlocks[i].isAlive = true;
                    // decrement deadCounter
                    deadCounter--;
                    NumberOfThreads += 1;
                    EndCriticalSection(IBit_State);
                    return NO_ERROR;

                }
            }
            // else
                /*
                Append the new thread to the end of the linked list
                * 1. Number of threads will refer to the newest thread to be added since the current index would be NumberOfThreads-1
                * 2. Set the next thread for the new thread to be the first in the list, so that round-robin will be maintained
                * 3. Set the current thread's nextTCB to be the new thread
                * 4. Set the first thread's previous thread to be the new thread, so that it goes in the right spot in the list
                * 5. Point the previousTCB of the new thread to the current thread so that it moves in the correct order
                */
                // 2
                threadControlBlocks[NumberOfThreads].nextTCB = &threadControlBlocks[0];
                // 3
                threadControlBlocks[NumberOfThreads-1].nextTCB = &threadControlBlocks[NumberOfThreads];
                // 4
                threadControlBlocks[0].previousTCB = &threadControlBlocks[NumberOfThreads];
                // 5
                threadControlBlocks[NumberOfThreads].previousTCB = &threadControlBlocks[NumberOfThreads-1];

                SetInitialStack(NumberOfThreads);

                threadStacks[NumberOfThreads][STACKSIZE-2] = (uint32_t)(threadToAdd);
                
                // add name and priority
                // threadControlBlocks[NumberOfThreads].threadName = name;
                threadControlBlocks[NumberOfThreads].priority = priority;
                strncpy(threadControlBlocks[NumberOfThreads].threadName, name, sizeof(threadControlBlocks[NumberOfThreads].threadName) - 1);
                threadControlBlocks[NumberOfThreads].threadName[sizeof(threadControlBlocks[NumberOfThreads].threadName) - 1] = '\0'; // Ensure null-termination
                
                threadControlBlocks[NumberOfThreads].ThreadID = NumberOfThreads;
                
                // the one thing my check above does not cover is if the last thread in the list was the one that was killed
                // Just check if dead counter is 0, if it is, then it is a completely new thread and needs to be tracked
                if(deadCounter == 0){
                    threadCounter++;
                }else{
                    // there are no longer any dead threads
                    deadCounter -= 1;
                }
                threadControlBlocks[NumberOfThreads].isAlive = true;
                NumberOfThreads += 1;
        }
        EndCriticalSection(IBit_State);
        return NO_ERROR;
    }

}

// sleep
// Puts current thread to sleep
// Param uint32_t "durationMS": how many systicks to sleep for
void sleep(uint32_t durationMS) {
    // Update time to sleep to
    // Set thread as asleep
    CurrentlyRunningThread->sleepCount = durationMS + SystemTime;
    CurrentlyRunningThread->asleep = true;

    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}


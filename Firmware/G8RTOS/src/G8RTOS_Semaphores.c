...

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// Unblocks all threads currently blocked on the semaphore.
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_SignalSemaphore(semaphore_t* s) {
    tcb_t *pt;
    IBit_State = StartCriticalSection();
    (*s) = (*s) + 1;
    if((*s) <= 0){
        pt = CurrentlyRunningThread->nextTCB; // search for one blocked on this
        while(pt->blocked != s){
            pt = pt->nextTCB;
    }
    pt->blocked = 0; // wakeup this one
    }
    EndCriticalSection(IBit_State);
}


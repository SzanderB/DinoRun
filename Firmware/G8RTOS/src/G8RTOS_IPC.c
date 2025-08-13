...

// G8RTOS_ReadFIFO
// Reads data from head pointer of FIFO.
// Param uint32_t "FIFO_index": Index of FIFO block
// Return: int32_t
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index) {
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].mutex));
    //check to see if there's data
    if(FIFOs[FIFO_index].currentSize == 0){
        G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));
        return -2;
    }
    // check if index is valid
    if(FIFO_index >= MAX_NUMBER_OF_FIFOS){
        G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));
        return -1;
    }
    int32_t data = *(FIFOs[FIFO_index].head);
    FIFOs[FIFO_index].head +=1;
    // if its at the end, loop
    if(FIFOs[FIFO_index].head == &(FIFOs[FIFO_index].buffer[FIFO_SIZE])){
        FIFOs[FIFO_index].head = &(FIFOs[FIFO_index].buffer[0]);
    }
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].currentSize));
    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));

    return data;
}

...

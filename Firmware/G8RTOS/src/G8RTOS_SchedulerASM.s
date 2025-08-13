...

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
;	- Saves current stack pointer to tcb
;	- Calls G8RTOS_Scheduler to get new tcb
;	- Set stack pointer to new stack pointer from new tcb
;	- Pops registers from thread stack
PendSV_Handler:

	.asmfunc
		CPSID I
	PUSH {R4-R11}

	LDR R4, RunningPtr
	LDR R5, [R4]
	STR SP, [R5]

	PUSH {R0, LR}

	BL G8RTOS_Scheduler

	POP {R0, LR}

	LDR R5, [R4]
	LDR SP, [R5]

	POP {R4-R11}

	CPSIE I

	BX LR


	.endasmfunc

	; end of the asm file
	.align
	.end

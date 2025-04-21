.section .text
.global set_led
.type set_led %function

set_led:
	PUSH {R4-R7, LR}
	LDR R4, =0x400FF004 // GPIOE_PSOR address (0x400FF004)
	LDR R5, =0x400FF008 // GPIOE_PCOR address (0x400FF008)
	MOVS R6, #0x08 // PTE3 mask (1 << 3)
	LDR R7, =0x00FFFFFF
	AND R0, R0, R7
	MOVS R1, #24 // Counter of the loop

loop:
	LSL R0, R0, #1
	BCS one_bit

zero_bit:
	STR R6, [R4] // PTE3 high
	NOP // T0H delay, 1 cycle
	STR R6, [R5] // PTE3 low
	// T0L delay 10 times
	NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    B decrement

 decrement:
 	SUB R1, R1, #1
 	BNE loop
 	POP {R4-R7, PC}

 one_bit:
 	STR R6, [R4] // PTE3 high
 	// T1H delay, 9 cycles
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	STR R6, [R5] // PTE3 low
 	// T1L delay, 10 cycles
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP
 	NOP



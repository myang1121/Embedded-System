.section .text
.global main


//Set up address constants for port

//Setting up LED
.equ 	PCRD5, 		0x4004C014 //PORTD_PCR5
.equ 	SCGC5, 		0x40048038 //SIM_SCGC5
.equ 	PDDRD,  	0x400FF0D4 //GPIOB_PDDR

//Turn LED on and off
.equ 	PCORD,  	0x400FF0C8 //GPIOB_PCOR
.equ 	PSORD,  	0x400FF0C4 //GPIOB_PSOR


.equ 	bit_twelve, 	0x00001000  // 1 << 12, for turning on port clock
.equ 	bit_eight, 	0x00000100  // 1 << 8,  for setting MUX to GPIO
.equ	bit_five, 	0x00000020  // 1 << 5,  for manipulating PIN

main:

	MOV   R0, #6 // Display up to n = 6
	BL fib // Calculate fib(n)
	BL    LEDSETUP // <- Call this once before LEDON and/or LEDOFF
	BL morse_digit // Display result of fib(n)
	B forever // End

/*
	// Testing
	LDR R4, =-6
	BL    LEDSETUP
	TEST_FIB: // From negative n value to 6
	MOV R0, R4
	BL fib
	BL morse_digit
	ADD R4, R4, #1 // Increment by 1
	CMP R4, #7 // Stop when reach n = 7
	BLT TEST_FIB
	B forever
*/
fib:
	PUSH {R4, R5, LR}
	CMP R0, #0
	//if n <= 0
	BLE FIB_ZERO
	//if n == 1
	CMP R0, #1
	BEQ FIB_ONE

	//F n-1
	SUB R4, R0, #1 //n-1
	PUSH {R0}
	MOV R0, R4
	BL fib
	MOV R5, R0
	POP {R0}
	//F n-2
	SUB R4, R0, #2 //n-2
	MOV R0, R4
	BL fib

	//Add F n-1 and F n-2
	ADD R0, R5, R0
	POP {R4, R5, PC}

FIB_ZERO:
	MOV R0, #0
	POP {R4, R5, PC}

FIB_ONE:
	MOV R0, #1
	POP {R4, R5, PC}

	
morse_digit:
	PUSH {R4, LR}
	CMP R0, #1 // if R0 == 1
	BNE NOT_DIGIT_1
	B DIGIT_1
	NOT_DIGIT_1:
	CMP R0, #2 // if R0 == 2
	BNE NOT_DIGIT_2
	B DIGIT_2
	NOT_DIGIT_2:
	CMP R0, #3 // if R0 == 3
	BNE NOT_DIGIT_3
	B DIGIT_3
	NOT_DIGIT_3:
	CMP R0, #4 // if R0 == 4
	BNE NOT_DIGIT_4
	B DIGIT_4
	NOT_DIGIT_4:
	CMP R0, #5 // if R0 == 5
	BNE NOT_DIGIT_5
	B DIGIT_5
	NOT_DIGIT_5:
	CMP R0, #6 // if R0 == 6
	BNE NOT_DIGIT_6
	B DIGIT_6
	NOT_DIGIT_6:
	CMP R0, #7 // if R0 == 7
	BNE NOT_DIGIT_7
	B DIGIT_7
	NOT_DIGIT_7:
	CMP R0, #8 // if R0 == 8
	BNE NOT_DIGIT_8
	B DIGIT_8
	NOT_DIGIT_8:
	CMP R0, #9 // if R0 == 9
	BNE NOT_DIGIT_9
	B DIGIT_9
	NOT_DIGIT_9:
	CMP R0, #0 // if R0 == 0
	BNE NOT_DIGIT_0
	B DIGIT_0
	NOT_DIGIT_0:
	B forever


DIGIT_1:
	BL DOT
	BL DASH
	BL DASH
	BL DASH
	BL DASH
	POP {R4, PC} // After program runs, enter this infinite loop


DIGIT_2:
	BL DOT
	BL DOT
	BL DASH
	BL DASH
	BL DASH
	POP {R4, PC}

DIGIT_3:
	BL DOT
	BL DOT
	BL DOT
	BL DASH
	BL DASH
	POP {R4, PC}

DIGIT_4:
	BL DOT
	BL DOT
	BL DOT
	BL DOT
	BL DASH
	POP {R4, PC}

DIGIT_5:
	BL DOT
	BL DOT
	BL DOT
	BL DOT
	BL DOT
	POP {R4, PC}

DIGIT_6:
	BL DASH
	BL DOT
	BL DOT
	BL DOT
	BL DOT
	POP {R4, PC}

DIGIT_7:
	BL DASH
	BL DASH
	BL DOT
	BL DOT
	BL DOT
	POP {R4, PC}

DIGIT_8:
	BL DASH
	BL DASH
	BL DASH
	BL DOT
	BL DOT
	POP {R4, PC}

DIGIT_9:
	BL DASH
	BL DASH
	BL DASH
	BL DASH
	BL DOT
	POP {R4, PC}

DIGIT_0:
	BL DASH
	BL DASH
	BL DASH
	BL DASH
	BL DASH
	POP {R4, PC}

end_morse:
	POP {R4, PC}


// Functions DOT, DASH DELAY
// Write loops
DOT:
	PUSH {LR} // Caller save register --> Callee can do whatever
	BL LEDON // LEDON
	BL DOT_LOOP // Controls dot duration
	BL LEDOFF // LEDOFF
	BL DOT_LOOP // Delay same as dot duration
	POP {PC}

DASH:
	PUSH {LR} // Caller save register --> Callee can do whatever
	BL LEDON // LEDON
	BL DASH_LOOP // Controls dash duration --> 3xdot duration
	BL LEDOFF // LEDOFF
	BL DOT_LOOP // Delay same as dot duration
	POP {PC}

DOT_LOOP:
	PUSH {R1}
	LDR R1, =#2194500 // Loop iterations = clock speed * duration = 20.9 MHz * 35 ms
DOT_NEXT:
	SUB R1, #1 // Decrement by 1, finish an iteration --> set Z flag when R1 == 0
	BNE DOT_NEXT
	POP {R1} // Restore R1
	BX LR // Be careful what LR is currently --> end function call

DASH_LOOP:
	PUSH {R1}
	LDR R1, =#6583500 // Loop iterations = clock speed * duration = 20.9 MHz * 3*35 ms
DASH_NEXT:
	SUB R1, #1 // Decrement by 1, finish an iteration --> set Z flag when R1 == 0
	BNE DASH_NEXT
	POP {R1} // Restore R1
	BX LR // Be careful what LR is currently --> end function call


// Call this function first to set up the LED -> once
// Configures microcontroller board to control LED
LEDSETUP:
	PUSH  {R4, R5}    // To preserve R4 and R5
	LDR   R4, =bit_twelve // Load the value 1 << 12
	LDR   R5, =SCGC5
	STR   R4, [R5]

	LDR   R4, =bit_eight  // MUX Port to be GPIO
	LDR   R5, =PCRD5
	STR   R4, [R5]

	LDR   R4, =bit_five
	LDR   R5, =PDDRD
	STR   R4, [R5]
	POP   {R4, R5}
	BX    LR

// Turn LED on and off functions
LEDON:
	PUSH  {R4, R5}
	LDR   R4, =bit_five
	LDR   R5, =PCORD
	STR   R4, [R5]
	POP   {R4, R5}
	BX    LR

LEDOFF:
	PUSH  {R4, R5}
	LDR   R4, =bit_five
	LDR   R5, =PSORD
	STR   R4, [R5]
	POP   {R4, R5}
	BX    LR

// Wait forever
forever:
	B		forever
	.end


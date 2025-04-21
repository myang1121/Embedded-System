#include "3140_concur.h" // Declarations for functions need to implement
#include <stdlib.h> // Standard library --> allow use memory allocation (malloc) and deallocate (free) functions
#include <MKL46Z4.h> // Interact with peripherals on the device (like Lab 2)

// Allocate and initialize a new process (pcb + stack)
// Create process_state data structure and save to process queue
int process_create (void (*f) (void), int n){

	// Stack pointer for the process --> data to manage process & pointer to next process
	unsigned int* sp;
	// Unsigned int* current_sp;


	// Allocate memory for a process_t structure
	process_t *new_process;
	// Process_t *current_process;

	// Allocate memory for new PCB
	new_process = (process_t *)malloc(sizeof(process_t));
	if (new_process == NULL){
		return -1; //error --> creation fail
	}

	// Create process that starts at function f with initial stack size of n
	// Allocates stack for process and set up stack's initial state
	sp = process_stack_init(f, n);
	if(sp == NULL){
		process_stack_free(sp, n); // Free memory allocated above --> must free() after doing malloc() or else error

		free(new_process);
		return -1; // Error
	}

	new_process -> sp = sp;
	new_process -> size = n;
	new_process -> next = NULL;

	// Enqueue newly created process_state to process queue
	enqueue(new_process, &process_queue);
	return 0; // Success


}

// Context switching
// The context switches should occur at a rate of about 100Hz.
void process_start (void){
	// Enable clock for PIT module --> bit 23: PIT --> clock enable (1)
	SIM -> SCGC6 |= SIM_SCGC6_PIT_MASK; // Macros from MKL46Z4.h
	// Turn on PIT --> bit 1: MDIS Module Disable (PIT Selection) --> clock for standard PIT timers enable (0)
	PIT -> MCR = 0;
	// Set the period of the timer --> LDVAL --> field 31:0 TSV timer start value can't be too small or large
	PIT -> CHANNEL[0].LDVAL = 1000; // Load time start value
	// Configure NVIC
	NVIC_EnableIRQ(PIT_IRQn); // IRQ = 22, Source Module = PIT in Interrupt Vector Assignments --> PIT single interrupt vector for all channels

	// Initialize 1st process
	process_begin(); // Start concurrency and use process_select to run 1st ready process

}

// Which process to run next --> scheduling mechanism (round robin)
unsigned int * process_select(unsigned int * cursp){
	// cursp --> current stack pointer for process just pre-empted w/ context at the top of the stack



	// NULL until process_start() is called

	// Check process queue --> not empty
	// If ready process
	if ((current_process_p != NULL) & (cursp != NULL)){
		// Save cursp to resume execution later (this process is selected again)
		current_process_p -> sp = cursp;
		enqueue(current_process_p, &process_queue); // Put at end of queue to run later
	}

	// Dequeue from process queue
	process_t *next = dequeue(&process_queue);

	// Check current process --> current_process_p
	// Check process queue --> empty
	// NULL if no currently running process
	// NULL if no processes left to run --> last process terminates
	if (next == NULL){
		current_process_p = NULL;
		return NULL;
	}

	// Update global variable to next process
	current_process_p = next;

	// Return value of stack pointer sp for next ready process (linked list --> next node pointer)
	return next -> sp;




}

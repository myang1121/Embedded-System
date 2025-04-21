#include "lock.h" // Declarations for functions need to implement
#include <stdlib.h> // Standard library --> allow use memory allocation (malloc) and deallocate (free) functions
#include <MKL46Z4.h> // Interact with peripherals on the device (like Lab 2)
#include "3140_concur.h"

// Initialize lock l
void l_init(lock_t* l) {
	l -> locked = 0; // Unlock, initialize as available lock
	l -> blocked_queue.head = NULL; // Initialize as NULL --> empty queue
}






void l_lock(lock_t* l) {
	// Atomicity
	NVIC_DisableIRQ(PIT_IRQn);
	while (l -> locked) { // Leave blocked queue if lock l not locked and is available to lock
		// When called, block the process until lock l is available --> process_select don't select blocked process


		// Maintain blocked processed in: a dedicated queue for each lock
		// ^should I achieve this by editing process.c and add a blocked_queue?
		// Revisit process initialization if process_state altered --> change in process_select --> process_t's "blocked field"
		current_process_p -> blocked = 1;

		// Switch execution to next ready process (process_blocked() in 3140.s)
		// Might need to use PIT_IRQHandler(), enqueue(), dequeue() to modify process queue and blocked process
		// ^or functions in process.c --> process_select(); to select next process
		enqueue(current_process_p, &l -> blocked_queue); // Add to blocked_queue

		// When process blocked, allow other process to run --> Enable interrupt for context switch
		NVIC_EnableIRQ(PIT_IRQn);

		// Handle blocked queue --> use process_blocked() provided in 3140.s
		process_blocked();

		NVIC_DisableIRQ(PIT_IRQn); // Atomicity recovered
	}

	// Leave blocked queue
	l -> locked = 1; // Lock process
	NVIC_EnableIRQ(PIT_IRQn); // End atomicity
}




// Release lock l along with first process that may be waiting on the lock --> fairness lock acquisition
// Leave blocking queue, return to process queue by process_select
// Remove from blocked queue
// Add to ready process queue
void l_unlock(lock_t* l) {
	NVIC_DisableIRQ(PIT_IRQn);


	// Blocked process woken up by unlock
	l -> locked = 0; // Unlock process

	// Update PCB
	process_t *process = dequeue(&l -> blocked_queue);
	if (process) {
		process -> blocked = 0; // Unblocked
		enqueue (process, &process_queue); // Return to process queue
	}
	NVIC_EnableIRQ(PIT_IRQn);

}

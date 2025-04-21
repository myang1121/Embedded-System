#include "lock.h" // Declarations for functions need to implement
#include <stdlib.h> // Standard library --> allow use memory allocation (malloc) and deallocate (free) functions
#include <MKL46Z4.h> // Interact with peripherals on the device (like Lab 2)
#include "3140_concur.h"


// General sequence of events:
// Define lock, initialize
// Create processes --> start processing
// If lock reached, atomically allocate resources or have process spin or block the process --> Safely process within critical section
// If unlock reached, atomically free resource  --> concurrently run non-blocked processes


// Initialize lock l
void l_init (lock_t *l) {
	l -> locked = 0; // Initialize lock as unlock (free)

}


// When called, grab lock l until lock is available --> busy-wait
// Consider where to lock resource (lock is 1, unlock is 0)
// Consider what to do if resource is already locked
void l_lock(lock_t* l) {
	// Try to grab lock until lock available
	while (1) { // Loop forever
		// On a single processor system, small atomic actions can be performed by disabling interrupts
		// Disable interrupt
		NVIC_DisableIRQ(PIT_IRQn);
		// Check if we have lock
		if (l -> locked == 0) { // Available lock
			l -> locked = 1; // Lock the lock
			// Enable interrupt --> atomic actions end
			NVIC_EnableIRQ(PIT_IRQn);
			break; // Leave busy-wait
		}
		// If lock is unavailable --> enable interrupt (atomic actions attempt ends) and loop again
		NVIC_EnableIRQ(PIT_IRQn);
	}

}


// Release lock l along with first process that may be waiting on the lock --> fairness lock acquisition
// Need to unlock the resource
void l_unlock(lock_t* l) {
	// To achieve atomicity, enable/disable interrupts as appropriate
	// Disable interrupt
	NVIC_DisableIRQ(PIT_IRQn);
	// Unlock
	l -> locked = 0;
	// Enable interrupt
	NVIC_EnableIRQ(PIT_IRQn);


}

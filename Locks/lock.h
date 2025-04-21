#ifndef __LOCK_H_INCLUDED__
#define __LOCK_H_INCLUDED__

#include "3140_concur.h"

typedef struct lock_ {
	volatile int locked; // Lock is 1, unlock is 0
	process_queue_t blocked_queue; // For blocked process
} lock_t;

/**
 * Initialises the lock structure
 *
 * @param l pointer to lock to be initialised
 */
void l_init(lock_t* l);

/**
 * Grab the lock or block the process until lock is available
 *
 * @param l pointer to lock to be grabbed
 */
void l_lock(lock_t* l);

/**
 * Release the lock along with the first process that may be waiting on
 * the lock. This ensures fairness wrt lock acquisition.
 *
 * @param l pointer to lock to be unlocked
 */
void l_unlock(lock_t* l);

#endif /* __LOCK_H_INCLUDED */

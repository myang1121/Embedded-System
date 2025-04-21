#include <stdlib.h>
#include "3140_concur.h"
#include <MKL46Z4.h>
#include "realtime.h"

/*
 * Use a different timer (PIT_1) to generate interrupts every millisecond (0.001s)
 * and update global variable current_time defined in realtime.h, which record the
 * time elapsed with respect to process_start()
 */

// Current_time initialize to 0, global variable
volatile realtime_t current_time = {0, 0};

// Generate interrupt for real-time clock, using PIT_1
void PIT1_Service(void) {
	// Increment time in msec
	current_time.msec += 1;
	// If msec >= 1000, increment sec and reset msec --> carry over
	if (current_time.msec >= 1000) {
		current_time.sec += 1;
		current_time.msec = 0;
	}
	// Clear PIT_1 interrupt flag
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF_MASK;
}


// Number of tasks that meet and miss their deadlines, global variables
int process_deadline_met = 0;
int process_deadline_miss = 0;

static void process_free(process_t *proc) {
	/* if (proc->is_realtime) {
		green_off_frdm();
	} */
	process_stack_free(proc->orig_sp, proc->n);
	free(proc);
}




// One queue for normal processes
// process_queue_t process_queue = {NULL};
// One queue for all real-time processes which are not ready (sorted by arrival time)
process_queue_t rt_unready_queue = {NULL};
// One queue for ready real-time processes (sorted by deadline)
process_queue_t rt_ready_queue = {NULL};


/* Starts up the concurrent execution */
void process_start (void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0; // PIT clock enable
	PIT->CHANNEL[0].LDVAL = 150000; // 15MHz / 100Hz
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; // To ensure PIT1 and PIT0 doesn't interfere when initialized

	// Initialize PIT timer 1 (PIT_1)
	PIT->CHANNEL[1].LDVAL = 14999; // 15MHz / 100Hz - 1 --> 1ms
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
	NVIC_EnableIRQ(PIT_IRQn);
	
	// When process_start() is called, reset current_time to zero
	current_time.sec = 0;
	current_time.msec = 0;

	if (is_empty(&rt_unready_queue) && is_empty(&rt_ready_queue) && is_empty(&process_queue)){
		//bail out fast if no processes were ever created
		return;
	}


	process_begin();
}

/* Create a new process */
int process_create (void (*f)(void), int n) {
	unsigned int *sp = process_stack_init(f, n);
	if (!sp) return -2; // stack allocation fail
	
	process_t *proc = (process_t*) malloc(sizeof(process_t));
	if (!proc) {
		process_stack_free(sp, n);
		return -1; // memory allocation fail
	}
	
	proc->sp = proc->orig_sp = sp;
	proc->n = n;
	
	enqueue(proc,&process_queue);
	
	return 0; // success
}

/* Create tasks with real-time constraints */
int process_rt_create(void (*f)(void), int n, realtime_t* start, realtime_t* deadline) {
	unsigned int *sp = process_stack_init(f, n);
	if (!sp) return -2; // stack allocation fail

	process_t *proc = (process_t*) malloc(sizeof(process_t));
	if (!proc) {
		process_stack_free(sp, n);
		return -1; // memory allocation fail
	}

	proc->sp = proc->orig_sp = sp;
	proc->n = n;
	proc->is_realtime = 1;
	proc->arrival_time = *start;

	// Determine absolute deadline di = ri + Di (absolute deadline)
	proc->deadline.sec = start->sec + deadline->sec;
	proc->deadline.msec = start->msec + deadline->msec;
	if (proc->deadline.msec >= 1000) {
		proc->deadline.sec += proc->deadline.msec/1000;
		proc->deadline.msec%=1000;
	}

	// Add task to real time unready queue
	process_t **head = &rt_unready_queue.head;
	while (1) { // Infinite loop
		// Sort base on arrival time of new process
		if (*head == NULL) { // End of list
			break;
		} else if ((*head)->arrival_time.sec < proc->arrival_time.sec) { // Arrives early
			head = &(*head)->next;
		} else if ((*head)->arrival_time.sec == proc->arrival_time.sec && (*head)->arrival_time.msec < proc->arrival_time.msec) {
			head = &(*head)->next;
		} else {
			break;
		}
	}
	// Add new process to queue
	proc->next = *head;
	*head = proc;

	return 0;
}

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/
unsigned int * process_select (unsigned int * cursp) {

	// Need to make sure there is no process in the unready queue before termination
	if (!cursp && current_process_p) {
		if (current_process_p->is_realtime) {
			// Compare current time with process deadline
			if (current_time.sec < current_process_p->deadline.sec || (current_time.sec == current_process_p->deadline.sec && current_time.msec < current_process_p->deadline.msec)) {
				process_deadline_met += 1; // Meet deadline, fi - di positive
			} else {
				process_deadline_miss += 1; // Miss deadline
			}
		}
		// Free resources
		process_free(current_process_p);
	} else if (cursp) {
		// Suspending a process which has not yet finished, save stack pointer
		current_process_p->sp = cursp;

		// Real time process --> add to ready real time queue
		if (current_process_p->is_realtime) {
			process_t **head = &rt_ready_queue.head;
			// Ready RT queue sort by deadline
			while (*head && ((*head)->deadline.sec < current_process_p->deadline.sec ||((*head)->deadline.sec == current_process_p->deadline.sec &&(*head)->deadline.msec < current_process_p->deadline.msec))){
				head = &(*head)->next;
			}
			current_process_p->next = *head;
			*head = current_process_p;
		} else {
			//Save state for non real time process and enqueue it
			enqueue(current_process_p,&process_queue);

		}
	}

	// Move real-time processes from unready queue to ready queue when arrival/request time reached
	while (rt_unready_queue.head) {
		process_t *proc = dequeue(&rt_unready_queue);
		// Check if process is ready
		// Current time >= arrival time
		if (current_time.sec > proc->arrival_time.sec || (current_time.sec == proc->arrival_time.sec && current_time.msec >= proc->arrival_time.msec)) {
			// Leave unready queue
			rt_unready_queue.head = proc->next;
			// Valid deadline
			if (current_time.sec < proc->deadline.sec || (current_time.sec == proc->deadline.sec && current_time.msec < proc->deadline.msec)) {
				// Add to ready queue
				proc->next = rt_ready_queue.head;
				rt_ready_queue.head = proc;
			} else {
				process_deadline_miss++;
				process_free(proc);
			}
		} else {
			enqueue(proc, &rt_unready_queue); // If not ready
			break;
		}

	}
	


	// Select a new process from the queue and make it current
	if (rt_ready_queue.head) {
		current_process_p = dequeue(&rt_ready_queue);
	} else {
		current_process_p = dequeue(&process_queue);
	}

	if (current_process_p) {
		// Launch the process which was just popped off the queue
		return current_process_p->sp;
	} else {
		// No process left, terminate
		return NULL;
	}
}

#ifndef PROCESS_H_
#define PROCESS_H_

struct process_state{
	unsigned int *sp; // Current stack pointer

	int size; // Size of stack allocation
	struct process_state * next; // For en/de-queue
};

typedef struct process_state process_t;


#endif /* PROCESS_H_ */


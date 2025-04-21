#ifndef PROCESS_H_
#define PROCESS_H_

struct process_state{
	unsigned int *sp;
	unsigned int *orig_sp;
	int n;
	int blocked;
	struct process_state * next;
};

typedef struct process_state process_t;


#endif /* PROCESS_H_ */

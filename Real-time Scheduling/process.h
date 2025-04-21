#ifndef PROCESS_H_
#define PROCESS_H_
#include "realtime.h"


struct process_state{
	unsigned int *sp;
	unsigned int *orig_sp;
	int n;
	struct process_state * next;
	/*
	 * Example fields that I may need to add, from discussion:
	 * int is_realtime;
	 * realtime_t arrival_time;
	 * realtime_t deadline;
	 */
	int is_realtime;
	realtime_t arrival_time;
	realtime_t deadline;
};

typedef struct process_state process_t;


#endif /* PROCESS_H_ */

#include "3140_concur.h"
#include "led.h"
#include "lock.h"

// Test for aggressively acquiring and releasing the lock
// Simulating a highly concentrated scenario of stressing the lock mechanism

// Expect to see red on board LED rapidly flicker, showing the lock is being successfully acquired and released
// by different processes in quick succession -> play with delay values
// Testing gain confidence for avoiding deadlock

// Global
lock_t l;

void process1(){
	for(int i =0; i < 100; i++){
		l_lock(&l);

		red_on_frdm();
		delay(150);
		red_off_frdm();

		l_unlock(&l);
		delay(150);
	}
}


int main(void){
	led_init();
	l_init(&l);

	for(int i = 0; i < 5; i++){
		if(process_create(process1, 30) < 0){
			return -1;
		}
	}

	process_start();
	while(1);
	return 0;
}



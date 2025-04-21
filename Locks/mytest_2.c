// Test cases --> last page of discussion lab 4
// This test case tests that safety, progress, fairness is maintained even in nested lock acquisition
// This test case ensures that deadlock is avoided (progress)
// The expected result of this test case is for the red, green, blue led to toggle (red turns on, green turns on,
// Blue turns on, then red turns off, green turns off, blue turns off, repeat) in sequential order, repeatedly.
#include "3140_concur.h"
#include "led.h"
#include "lock.h"


lock_t l_1;
lock_t l_2;

void p1(void){
	while(1){
		/*CS*/
		l_lock(&l_1);
		l_lock(&l_2);
		red_toggle_frdm();
		delay(500);
		l_unlock(&l_2);
		l_unlock(&l_1);
		/*NCS*/
		delay(300);
	}
}

void p2(void){
	while(1){
		/*CS*/
		l_lock(&l_1);
		l_lock(&l_2);
		green_toggle_frdm();
		delay(500);
		l_unlock(&l_2);
		l_unlock(&l_1);
		/*NCS*/
		delay(300);
	}
}

void p3(void){
	while(1){
		/*CS*/
		l_lock(&l_1);
		blue_toggle();
		delay(500);
		l_unlock(&l_1);
		/*NCS*/
		delay(300);
	}
}

int main(void){
	led_init();

	l_init(&l_1);
	l_init(&l_2);

	if (process_create (p1,20) < 0) {
	 	return -1;
	}
	if (process_create (p2,20) < 0) {
	 	return -1;
	}
	if (process_create (p3,20) < 0) {
		 	return -1;
		}


	process_start();
	while(1);
	return 0;
}

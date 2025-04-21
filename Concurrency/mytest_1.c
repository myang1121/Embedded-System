#include "3140_concur.h"
#include "led.h"

// This test case tests on when there is only one call to process_create();
void p1 (void) //process 1
{
	int i;
	for (i=0; i < 10; i++) { // Toggles 10 times --> so blink 5 times
		delay(500);
    	red_toggle_frdm(); // Runs every second or 1/2 second toggle
	}
}


int main (void){
  led_init();
  set15MHz();

 if (process_create (p1,32) < 0) { // Always put 32, but idk what it is...bits, bytes?
 	return -1;
 }

 process_start (); // Twice as fast as before because p1 finishes --> now if p2, p2, p2, p2

 /*
This tests for non-trivial edge cases of the scheduler's behavior.
This test tests that when process_start() runs, only one process starts and continues running.
Even after multiple interrupts, the process selected is still p1.
current_process_p always point to process p1
*/

 return 0;
}

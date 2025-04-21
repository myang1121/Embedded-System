#include "3140_concur.h"
#include "led.h"

// There is a total of 4 processes, with the 1st one very long (need to put back to queue, round robin), the second one terminating immediately,
// The third one also very long, and the fourth one running forever, not terminating.
void p1 (void) //process 1 - very long
{
	int i;
	for (i=0; i < 40; i++) { // Will light up 20 times
		delay(500);
    	red_toggle_frdm(); // Runs every second or 1/2 second toggle
	}
}

void p2 (void) // Process 2 --> terminates immediately
{
	// Nothing happen
}

void p3 (void) //process 3 --> same as p1, but green led
{
	int i;
	for (i=0; i < 40; i++) {
		delay(500);
    	green_toggle_frdm();

	}
}

// Continuous testing --> adjust red's intensity
volatile grb32_t val = {0, 0, 0, 0};
void p4 (void) // Process 4 --> Run forever
{
	int up = 1;
	while (1) {
		if (up) {
			val.red += 2;
			if (val.red > 200) {
				up = 0;
			}
		} else {
			val.red -= 2;
			if (val.red < 10) {
				up = 1;
			}
		}
		__disable_irq();
		set_led(val);
		__enable_irq();
		delay(10);
	}

}



int main (void){
  led_init();
  set15MHz();

 if (process_create (p1,32) < 0) {
 	return -1;
 }
 if (process_create (p2,32) < 0) {
  	return -1;
  }
 if (process_create (p3,32) < 0) {
  	return -1;
  }
 if (process_create (p4,32) < 0) {
  	return -1;
  }


 process_start (); // Twice as fast as before because p1 finishes --> now if p2, p2, p2, p2

 /*
After process_start p1 and p2 should be running concurrently.
Both LEDs should blink. The process with the red LED should
finish first, and after that the green LED should blink a few
more times, but twice as fast.
*/

 return 0;
}

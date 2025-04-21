#include "ECE3140Lab2.h"
#include <MKL46Z4.h>
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

int main(void) {
	set15MHz(); // Set system clock
	leds_init_frdm(); // Initialize board LED

    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable PIT clock
    PIT->MCR = 0x00; // Configure PIT module
    PIT->CHANNEL[0].LDVAL = 14999999; // Configure PIT0 for 1-second interval
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK;
    while(1) {
        if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) { // Poll TFLG0.TIF flag
            // Clear flag, toggle LED
            PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
            red_toggle_frdm();
        }
    }
}

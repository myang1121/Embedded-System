#include "ECE3140Lab2.h"
#include <MKL46Z4.h>
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

void PIT_IRQHandler(void) {
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
		green_on_frdm(); // Turn LED green on
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	}
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF_MASK;
		green_off_frdm(); // Turn green LED off
		PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	}
}


int main(void) {
	set15MHz(); // Set system clock
	leds_init_frdm(); // Initialize board LED

    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable PIT clock
    PIT->MCR = 0x00; // Configure PIT module, enable timer
    PIT->CHANNEL[0].LDVAL = 14999999;

    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    // ^Enable interrupt
    PIT->CHANNEL[1].LDVAL = 1499999;
    PIT->CHANNEL[1].TCTRL = PIT_TCTRL_TIE_MASK;
    // ^Enable interrupt
    NVIC_EnableIRQ(PIT_IRQn);
    __enable_irq();
    // Toggle red LED w/ 1 second delay
    while(1) {
    	red_toggle_frdm();
    	delay(1000);
    }
}


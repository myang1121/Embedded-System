#include "led.h"
#include <MKL46Z4.h>
#include <stdint.h> // Include for int types

volatile uint8_t target_red = 0;
volatile uint8_t target_green = 0;
volatile uint8_t target_blue = 0;
volatile uint16_t cycle_counter = 0;

// PIT Interrupt Handler for Part 5, color mixing
void PIT_IRQHandler(void)
{
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
		(cycle_counter < target_red)?red_on():red_off();
		(cycle_counter < target_green)?green_on():green_off();
		(cycle_counter < target_blue)?blue_on():blue_off();
	}
}

void led_init(void)
{
	// Enable clock for port E
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Configure PTE2, PTE6, PTE16 for LED, GPIO (MUX = 001)
	PORTE->PCR[2] = PORT_PCR_MUX(1); // Red
	PORTE->PCR[6] = PORT_PCR_MUX(1); // Green
	PORTE->PCR[16] = PORT_PCR_MUX(1); // Blue

	// Set output pins using Port data direction register (PDDR)
    PTE->PDDR |= (1<<2)|(1<<6)|(1<<16);

    // Initialize LED
    all_leds_off();
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR = 0x00;
    PIT->CHANNEL[0].LDVAL = 15359; // 15MHz/15360 = 977Hz (~1.024ms)
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK|PIT_TCTRL_TEN_MASK;
    NVIC_EnableIRQ(PIT_IRQn);
    __enable_irq();
}



// Port Clear Output Register (PCOR) --> to turn LED ON
void red_on(void)
{
	PTE->PCOR = (1<<2);
}
// Port Set Output Register (PSOR) --> to turn LED OFF
void red_off(void)
{
	PTE->PSOR = (1<<2);
}
// Port Toggle Output Register (PTOR) ; write a one to PTOR, toggle (invert) it
void red_toggle(void)
{
	PTE->PTOR = (1<<2);

}

void blue_on(void)
{
	PTE->PCOR = (1<<16);

}
void blue_off(void)
{
	PTE->PSOR = (1<<16);
}
void blue_toggle(void)
{
	PTE->PTOR = (1<<16);

}

void green_on(void)
{
	PTE->PCOR = (1<<6);

}
void green_off(void)
{
	PTE->PSOR = (1<<6);

}
void green_toggle(void)
{
	PTE->PTOR = (1<<6);

}

void all_leds_off(void)
{
	PTE->PSOR = (1<<2)|(1<<6)|(1<<16); //Set to turn off LED

}

void led_off(void)
{
	grb32_t off = {0, 0, 0, 0};
	set_led(off);
}
void set_leds(grb32_t * rgb_vals, uint32_t num_led)
{
	for(uint32_t i = 0; i < num_led; i++) {
		set_led(rgb_vals[i]);
	}
	 volatile uint32_t cycles = 750;
	 //^volatile key word, compiler won't optimize it and will read memory address every time
	 while(cycles--) __asm__ volatile("nop");
}
void leds_off(uint32_t num_led)
{
	grb32_t off = {0, 0, 0, 0};
	for(uint32_t i = 0; i < num_led; i++) {
		set_led(off);
	}
	volatile uint32_t cycles = 750;
	while(cycles--) __asm__ volatile("nop");
}
void set_led0(grb32_t rgb_val)
{
	target_red = rgb_val.red;
	target_green = rgb_val.green;
	target_blue = rgb_val.blue;
	/**rgb_val.red ? red_on() : red_off();
	rgb_val.green ? green_on() : green_off();
	rgb_val.blue ? blue_on() : blue_off();
	*/
}






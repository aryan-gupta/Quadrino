#pragma once

register uint8_t rr2 asm ("r2"); // set aside this register for sreg in isr
register uint8_t rr4 asm ("r4");
register uint8_t rr5 asm ("r5");

const uint8_t ESC_FL_DOWN = 0b11101111; // 0xEF
const uint8_t ESC_FR_DOWN = 0b11011111; // 0xDF
const uint8_t ESC_BL_DOWN = 0b10111111;
const uint8_t ESC_BR_DOWN = 0b01111111;

uint8_t escADown = 0, escBDown = 0;

// https://electronics.stackexchange.com/questions/117430
// THis link helped me alot and I keep losing the link
// Finally added it here
// #define USE_COMP_INLINE_ASM
#ifdef USE_COMP_INLINE_ASM

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"push r2" "\n\t"
		"in r2, __SREG__"  "\n\t" // Figure out why we need this
		"push r4" "\n\t"
		"push r5" "\n\t"
		// Set esc pulse to low
		"in	r4, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r5, %0"       "\n\t" 
		"and r5, r4"      "\n\t"
		"out 0x0b, r5"     "\n\t" // (I will figure out later how to soft code it)
		
		// Reset registers
		"pop r5" "\n\t"
		"pop r4" "\n\t"
		"out __SREG__, r2" "\n\t"
		"pop r2" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escADown) // Inputs
		: "r2", "r4", "r5" // Clobber list
	);
}

ISR(TIMER1_COMPB_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"push r2" "\n\t"
		"in r2, __SREG__"  "\n\t" // Figure out why we need this
		"push r4" "\n\t"
		"push r5" "\n\t"
		
		// Set esc pulse to low
		"in	r4, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r5, %0"       "\n\t" 
		"and r5, r4"      "\n\t"
		"out 0x0b, r5"     "\n\t" // (I will figure out later how to soft code it)
		
		// Reset registers
		"pop r5" "\n\t"
		"pop r4" "\n\t"
		"out __SREG__, r2" "\n\t"
		"pop r2" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escBDown) // Inputs
		: "r2", "r4", "r5" // Clobber list
	);

}

#else
	
ISR(TIMER1_COMPA_vect) {
	PORTD &= escADown;
}

ISR(TIMER1_COMPB_vect) {
	PORTD &= escBDown;
}

#endif

void 
output_esc_pulse(uint8_t escA, uint16_t ticksA,uint8_t escB, uint16_t ticksB) {
	uint16_t ctick = TCNT1 - 4;
	PORTD |= escA xor escB; // hehe -- pretty much gets which escs ports/pulses are going to be set high
	
	uint16_t escA_tick = ctick + ticksA;
	uint16_t escB_tick = ctick + ticksB;
	
	// The 16bit timer will loop back every 0.0327 seconds
	// (16M / 8) / 2^16 = overflows per second
	// There are 16 million ticks per second, the prescaler is 8
	// so there will be 2 million counts every second. this divided by 
	// the max timer1 is overflows per second
	
	// Compare A will handle one of the escs
	OCR1A = escA_tick; // We want the lower 16bits
	escADown = escA;
	
	// Compare B will handle the other
	OCR1B = escB_tick; // We want the lower 16bits
	escBDown = escB;
	
	// We want to enable compare interupt here
	TIFR1 &= 0b11111001;
	TIMSK1 |= 0b110;
}

void finish_esc_pulse() {
	while (PORTD >= 16); // wait for thhe signals to finish sending
	
	// We want to turn off the compare inturupt in case 
	// we loop back and hit it in while doing somthing else
	// The inturupt already turn off the inturupt but do it again
	// here for saftey. We might remove it from the inturupt and 
	// just do it here, lets see how it goes. 
	TIMSK1 &= 0b11111001;
}

void phase_1(void (*callback)()) {
	TCNT1 = 0;
	callback();
	while (TCNT1 < PHASE1_TICKS);
}

void phase_2(void (*callback)()) {
	uint16_t e = TCNT1 + PHASE2_TICKS;
	output_esc_pulse(ESC_FL_DOWN, escfl, ESC_FR_DOWN, escfr);
	callback();
	finish_esc_pulse();
	while (TCNT1 < e);
}

void phase_3(void (*callback)()) {
	uint16_t e = TCNT1 + PHASE3_TICKS;
	output_esc_pulse(ESC_BL_DOWN, escbl, ESC_BR_DOWN, escbr);
	callback();
	finish_esc_pulse();
	while (TCNT1 < e);
	
	loop_elapsed = TCNT1 / 2000000.0;
}

void do_all_phases(void (*fa)(), void (*fb)(), void (*fc)()) {
	phase_1(fa);
	phase_2(fb);
	phase_3(fc);
}

void calibrate_escs(void (*fa)(), void (*fb)(), void (*fc)()) {
	escfl = escfr = escbl = escbr = 4000;
	do_all_phases(dummy, fa, fb);
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		do_all_phases(dummy, fc, fc);
	}
	
	for (uint16_t ii = 4000; ii >= 2000; ii -= 5) {
		escfl = escfr = escbl = escbr = ii;
		do_all_phases(dummy, fc, fc);
	}
}

void output_empty_pulse(void (*fa)(), void (*fb)(), void (*fc)()) {
	escfl = escfr = escbl = escbr = 2000;
	do_all_phases(fa, fb, fc);
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		do_all_phases(dummy, dummy, dummy);
	}
}

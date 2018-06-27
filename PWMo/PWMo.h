#pragma once

register uint8_t rr3 asm ("r3"); // set aside this register for sreg in isr
register uint8_t rr4 asm ("r4");
register uint8_t rr5 asm ("r5");

const uint8_t ESC_FL_DOWN = 0b11101111; // 0xEF
const uint8_t ESC_FR_DOWN = 0b11011111; // 0xDF
const uint8_t ESC_BL_DOWN = 0b10111111;
const uint8_t ESC_BR_DOWN = 0b01111111;

uint8_t escADown = 0, escBDown = 0;

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"in r3, __SREG__"  "\n\t" // Figure out why we need this
		
		// Set esc pulse to low
		"in	r4, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r5, %0"       "\n\t" 
		"and r5, r4"      "\n\t"
		"out 0x0b, r5"     "\n\t" // (I will figure out later how to soft code it)
		
		// Reset registers
		"out __SREG__, r3" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escADown) // Inputs
		: "r3", "r4", "r5" // Clobber list
	);
}

ISR(TIMER1_COMPB_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"in r3, __SREG__"  "\n\t" // Figure out why we need this
		
		// Set esc pulse to low
		"in	r4, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r5, %0"       "\n\t" 
		"and r5, r4"      "\n\t"
		"out 0x0b, r5"     "\n\t" // (I will figure out later how to soft code it)
		
		// Reset registers
		"out __SREG__, r3" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escBDown) // Inputs
		: "r3", "r4", "r5" // Clobber list
	);

}

void output_esc_pulse(uint8_t escA, uint16_t ticksA, uint8_t escB, uint16_t ticksB) {
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

void calibrate_escs() {
	uint16_t e, s;
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		s = TCNT1;
		e = s + PHASE1_TICKS;
		// Do somthing productive here
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, 4000, ESC_FR_DOWN, 4000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_BL_DOWN, 4000, ESC_BR_DOWN, 4000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
	// Phase -- P1   >> P2   >> P3
	// Ticks -- 4500 >> 4500 >> 4500
	// Int   -- iBus >> ESC1 >> ESC2
	// Exec  -- iBus >> MPU  >> PID
	for (uint16_t ii = 4000; ii >= 2000; ii -= 5) {
		s = TCNT1;
		e = s + PHASE1_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, ii, ESC_FR_DOWN, ii);
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		output_esc_pulse(ESC_BL_DOWN, ii, ESC_BR_DOWN, ii);
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
}

void output_empty_pulse() {
	uint16_t e, s;
	for (uint16_t ii = 0; ii < 750; ++ii) {
		s = TCNT1;
		e = s + PHASE1_TICKS;
		// Do somthing productive here
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_FL_DOWN, 2000, ESC_FR_DOWN, 2000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_esc_pulse(ESC_BL_DOWN, 2000, ESC_BR_DOWN, 2000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
}
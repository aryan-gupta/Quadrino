#define DEBUG

#pragma once

const uint16_t ESC_LOW = 10000;
const uint8_t ESC_FR_DOWN = 0b11011111; // 0xDF
const uint8_t ESC_FL_DOWN = 0b11101111; // 0xEF
const uint8_t ESC_BR_DOWN = 0b01111111;
const uint8_t ESC_BL_DOWN = 0b10111111;

const uint16_t PHASE1_TICKS = 4500;
const uint16_t PHASE2_TICKS = 4500;
const uint16_t PHASE3_TICKS = 4500;

uint8_t escADown = 0, escBDown = 0;

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"in __tmp_reg__, __SREG__"  "\n\t" // Figure out why we need this
		"push r24"          "\n\t"
		"push r25"          "\n\t"
		
		// Set esc pulse to low
		"in	r25, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r24, %0"       "\n\t" 
		"and r24, r25"      "\n\t"
		"out 0x0b, r24"     "\n\t" // (I will figure out later how to soft code it)
		
		// Turn off this inturupt
		"lds r24, 0x006F"   "\n\t" // The offset for TIMSK1 is 0x6f
		"andi r24, 0xFD"    "\n\t"
		"sts 0x006F, r24"   "\n\t" // This op needs a 16-bit address
		
		// Reset registers
		"pop r25"           "\n\t"
		"pop r24"           "\n\t"
		"out __SREG__, __tmp_reg__" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escADown) // Inputs
		: "r24", "r25" // Clobber list
	);
}

ISR(TIMER1_COMPB_vect, ISR_NAKED) {
	asm volatile (
		// Setup registers
		"in __tmp_reg__, __SREG__"  "\n\t" // Figure out why we need this
		"push r24"          "\n\t"
		"push r25"          "\n\t"
		
		// Set esc pulse to low
		"in	r25, 0x0b"      "\n\t" // PORTD is address 0x0b
		"lds r24, %0"       "\n\t" 
		"and r24, r25"      "\n\t"
		"out 0x0b, r24"     "\n\t" // (I will figure out later how to soft code it)
		
		// Turn off this inturupt
		"lds r24, 0x006F"   "\n\t" // The offset for TIMSK1 is 0x6f
		"andi r24, 0xFB"    "\n\t"
		"sts 0x006F, r24"   "\n\t" // This op needs a 16-bit address
		
		// Reset registers
		"pop r25"           "\n\t"
		"pop r24"           "\n\t"
		"out __SREG__, __tmp_reg__" "\n\t"
		
		// return
		"reti"              "\n\t"

		:  // Outputs
		: "m" (escBDown) // Inputs
		: "r24", "r25" // Clobber list
	);

}

void output_esc_pulse(uint8_t escA, uint16_t ticksA, uint8_t escB, uint16_t ticksB) {
	uint32_t ctick = TCNT1;
	PORTD |= escA xor escB; // hehe -- pretty much gets which escs ports/pulses are going to be set high
	
	uint32_t escA_tick = ctick + ticksA;
	uint32_t escB_tick = ctick + ticksB;
	
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

void setup_timer() {
	// Timer0
	TCCR0B = 0x0; // disable Timer0
	TIMSK0 = 0x0;
	TCNT0 = 0;
	
	// Timer1
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B = (1 << CS11); // turn the prescaler to 8 (20.14.2 pg173)
	TCCR1C = 0;
	// TIMSK1 |= 0b1; // enable interrupt
	TCNT1 = 0;
	
	// Timer2
	TCCR2B = 0x0; // disable Timer2
	TIMSK2 = 0x0;
	TCNT2 = 0;
}

void setup_pins() {
	// setup output pins
	DDRD |= 0b11110000; // set pins 4:7 as output
}

void calibrate_escs() {
	uint16_t e, s;
	
	for (uint16_t ii = 0; ii < 750; ++ii) {
		s = TCNT1
		e = s + PHASE1_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_set_esc_pulse(ESC_FL_DOWN, 4000, ESC_FR_DOWN, 4000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_set_esc_pulse(ESC_BL_DOWN, 4000, ESC_BR_DOWN, 4000);
		// Do somthing productive here
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
	// Phase -- P1   >> P2   >> P3
	// Ticks -- 4500 >> 4500 >> 4500
	// Int   -- iBus >> ESC1 >> ESC2
	// Exec  -- iBus >> MPU  >> PID
	for (uint16_t ii = 4000; ii > 2000; ii -= 5) {
		s = TCNT1
		e = s + PHASE1_TICKS;
		while (TCNT1 < e);
		
		s = TCNT1;
		output_set_esc_pulse(ESC_FL_DOWN, ii, ESC_FR_DOWN, ii);
		finish_esc_pulse();
		e = s + PHASE2_TICKS;
		while (TCNT1 < e);
		
		output_set_esc_pulse(ESC_BL_DOWN, ii, ESC_BR_DOWN, ii);
		finish_esc_pulse();
		e = s + PHASE3_TICKS;
		while (TCNT1 < e);
	}
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
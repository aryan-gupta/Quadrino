

#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
#include "PWMo.h"
#define DEBUG

void dummy() {
	return;
}

void setup() {
	Serial.begin(BAUD_RATE);
	Serial.print("\n\n\n\n");
	setup_pins();
	setup_timer();
	calibrate_escs(dummy, dummy, dummy);
	output_empty_pulse(dummy, dummy, dummy);
}


void loop() {
	uint16_t s, e;
	
	s = TCNT1;
	e = s + PHASE1_TICKS;
	// Do somthing productive here
	while (TCNT1 < e);
	
	s = TCNT1;
	output_esc_pulse(ESC_FL_DOWN, 3500, ESC_FR_DOWN, 2500);
	// Do somthing productive here
	finish_esc_pulse();
	e = s + PHASE2_TICKS;
	while (TCNT1 < e);
	
	s = TCNT1;
	output_esc_pulse(ESC_BL_DOWN, 3500, ESC_BR_DOWN, 2500);
	// Do somthing productive here
	finish_esc_pulse();
	e = s + PHASE3_TICKS;
	while (TCNT1 < e);
}

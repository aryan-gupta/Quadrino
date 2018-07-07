

#include "C:/Users/Aryan/Projects/Quadrino/FlightCtrl/FlightCtrl.h"
#include "PWMo.h"
#define DEBUG

void setup() {
	Serial.begin(BAUD_RATE);
	Serial.print("\n\n\n\n");
	setup_pins();
	setup_timer();
	calibrate_escs(dummy, dummy, dummy);
	output_empty_pulse(dummy, dummy, dummy);
}


void loop() {
	escfl = escfr = escbl = escbr = 2500;
	do_all_phases(dummy, dummy, dummy);
}

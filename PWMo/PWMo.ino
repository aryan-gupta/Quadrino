
#include "PWMo.h"
#define DEBUG

void setup() {
#ifdef DEBUG
	Serial.begin(9600);
	Serial.print("\n\n\n\n");
#endif
	setup_pins();
	setup_timer();
	calibrate_escs();
}


void loop() {
	uint32_t e = TCNT1 + ESC_LOW;
	
	uint16_t escfr = 2400;
	uint16_t escfl = 2800;
	uint16_t escbr = 3200;
	uint16_t escbl = 3600;
	
	start_esc_pulse();
	finish_esc_pulse();
	
	while (TCNT1 < e);
}

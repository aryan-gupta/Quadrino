
uint16_t T1_MSB = 0;
uint32_t escfr_tick = 0, escfl_tick = 0, escbr_tick = 0, escbl_tick = 0;
uint16_t cmpAother = 0, cmpBother = 0;


ISR(TIMER1_COMPA_vect) {
	uint32_t ticks = get_ticks(); // run a profiler on this
	
	Serial.println(ticks);
	Serial.println(escfl_tick);
	Serial.println(escfr_tick);
	
	if (escfl_tick <= ticks) {
		PORTD &= 0b11011111;
		OCR1A = cmpAother;
	}
	
	if (escfr_tick <= ticks) {
		PORTD &= 0b11101111;
		OCR1A = cmpAother;
	}
	
	if (PORTD & 0b00110000 == 0) { // if both are low then turn off this inturupt
		TIMSK1 &= 0b11111101;
	}
	
	Serial.println(get_ticks());
	Serial.println('\n');
}

ISR(TIMER1_COMPB_vect) {
	uint32_t ticks = get_ticks(); // run a profiler on this
	if (escbl_tick <= ticks) {
		PORTD &= 0b01111111;
		OCR1B = cmpBother;
	}
	
	if (escbr_tick <= ticks) {
		PORTD &= 0b10111111;
		OCR1B = cmpBother;
	}
	
	if (PORTD & 0b11000000 == 0) { // if both are low then turn off this inturupt
		TIMSK1 &= 0b11111011;
	}
}

ISR(TIMER1_OVF_vect) {
	++T1_MSB;
	TIFR1 &= 0b11111110; // turn off the ov flag
}

uint32_t get_ticks() {
	return (uint32_t(T1_MSB) << 16) | TCNT1;
}

void setup() {
	Serial.begin(9600);
	Serial.print("\n\n\n");
	
	// Timer1
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B = (1 << CS11); // turn the prescaler to 8 (20.14.2 pg173)
	TCCR1C = 0;
	TIMSK1 |= 0b1;
	TCNT1 = 0;
}

void pulse() {
	uint16_t escfr = 3800;
	uint16_t escfl = 2600;
	uint16_t escbr = 3100;
	uint16_t escbl = 2100;
	
	uint32_t curTime = get_ticks();
	PORTD |= 0b11110000;
	escfr_tick = escfr + curTime;
	escfl_tick = escfl + curTime;
	escbr_tick = escbr + curTime;
	escbl_tick = escbl + curTime;
	
	// The 16bit timer will loop back every 0.0327 seconds
	// (16M / 8) / 2^16 = overflows per second
	// There are 16 million ticks per second, the prescaler is 8
	// so there will be 2 million counts every second. this divided by 
	// the max timer1 is overflows per second
	
	// Compare A will handle the front l/r escs
	if (escfr_tick < escfl_tick) {
		cmpAother = escfl_tick; // & 0xFFFF;
		OCR1A     = escfr_tick; // & 0xFFFF; // We want the lower 16bits
	} else {
		cmpAother = escfr_tick; // & 0xFFFF;
		OCR1A     = escfl_tick; // & 0xFFFF; // We want the lower 16bits
	}
	
	// Serial.print(OCR1A);
	
	// Compare B will hangle the back l/r escs
	if (escbr_tick < escbl_tick) {
		cmpBother = escbl_tick; // & 0xFFFF;
		OCR1B     = escbr_tick; // & 0xFFFF; // We want the lower 16bits
	} else {
		cmpBother = escbr_tick; // & 0xFFFF;
		OCR1B     = escbl_tick; // & 0xFFFF; // We want the lower 16bits
	}
	
	// We want to enable compare interupt here
	TIFR1 &= 0b11111001;
	TIMSK1 |= 0b010;
	
	while (PORTD & 0b00110000 != 0); // wait for thhe signals to finish sending
	// We want to turn off the compare inturupt in case 
	// we loop back and hit it in while doing somthing else
	TIMSK1 &= 0b11111101;
	
}

void loop() {
	pulse();
}
#define DEBUG

const uint16_t ESC_LOW = 10000;

uint16_t escfr_tick = 0, escfl_tick = 0, escbr_tick = 0, escbl_tick = 0;
uint16_t cmpAother = 0, cmpBother = 0;
uint8_t escADown, escAODown;

uint16_t T1_MSB = 0;
uint32_t loop_timer_prev = 0;
float loop_elapsed = 0;

ISR(TIMER1_COMPA_vect) {
	uint8_t port = escADown;
	escADown = escAODown;	
	// if both are low then turn off this inturupt
	// escODown ill only be 0 when this function runs twice
	if (escADown = 0)
		TIMSK1 &= 0b11111101;
	
	OCR1A = cmpAother; 
	escAODown = 0;
	PORTD &= port;
	
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
	
	if ((PORTD & 0b11000000) == 0) { // if both are low then turn off this inturupt
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

void output_set_esc_pulse(uint16_t us) {
	uint16_t escfr = us;
	uint16_t escfl = us;
	uint16_t escbr = us;
	uint16_t escbl = us;
	
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
		cmpAother = escfl_tick;
		OCR1A     = escfr_tick; // We want the lower 16bits
	} else {
		cmpAother = escfr_tick;
		OCR1A     = escfl_tick; // We want the lower 16bits
	}
	
	// Compare B will hangle the back l/r escs
	if (escbr_tick < escbl_tick) {
		cmpBother = escbl_tick;
		OCR1B     = escbr_tick; // We want the lower 16bits
	} else {
		cmpBother = escbr_tick;
		OCR1B     = escbl_tick; // We want the lower 16bits
	}
	
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
	TIMSK1 |= 0b1;
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

void setup() {
#ifdef DEBUG
	Serial.begin(9600);
	Serial.print("\n\n\n\n");
#endif
	setup_pins();
	setup_timer();
	
	// for (uint16_t ii = 0; ii < 1000; ++ii) {
		// uint32_t e = get_ticks() + 10000;
		// output_set_esc_pulse(4000);
		// finish_esc_pulse();
		// while (get_ticks() < e);
	// }
	
	for (uint16_t ii = 0; ii < 1000; ++ii) {
		output_set_esc_pulse(2000);
		finish_esc_pulse();
		while (get_ticks() < e);
	}
}

uint16_t gg = 2000;
bool inc = true;

void start_esc_pulse() {
	if (inc) {
		++gg;
	} else {
		--gg;
	}
	
	if (gg == 3000 or gg == 2000) inc = !inc;
	
	
	
	uint16_t escfr = gg;
	uint16_t escfl = gg;
	uint16_t escbr = gg;
	uint16_t escbl = gg;
	
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
		cmpAother = escfl_tick;
		OCR1A     = escfr_tick; // We want the lower 16bits
		escADown  = 0b11101111;
		escAODown = 0b11011111;
	} else {
		cmpAother = escfr_tick;
		OCR1A     = escfl_tick; // We want the lower 16bits
		escADown  = 0b11011111;
		escAODown = 0b11101111;
	}
	
	// Compare B will hangle the back l/r escs
	if (escbr_tick < escbl_tick) {
		cmpBother = escbl_tick;
		OCR1B     = escbr_tick; // We want the lower 16bits
	} else {
		cmpBother = escbr_tick;
		OCR1B     = escbl_tick; // We want the lower 16bits
	}
	
	// We want to enable compare interupt here
	TIFR1 &= 0b11111001;
	TIMSK1 |= 0b110;
}

void finish_esc_pulse() {
	while (PORTD >= 16); // wait for thhe signals to finish sending
	
	// We want to turn off the compare inturupt in case 
	// we loop back and hit it in while doing somthing else
	TIMSK1 &= 0b11111001;
}

void loop() {
	uint32_t e = get_ticks() + ESC_LOW;
	
	start_esc_pulse();
	finish_esc_pulse();
	
	while (get_ticks() < e);
}
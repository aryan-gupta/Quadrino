
uint16_t T1_MSB = 0;

ISR(TIMER1_OVF_vect) {
	++T1_MSB;
	TIFR1 &= 0b11111110; // turn off the ov flag
}

uint32_t get_ticks() {
	return (uint32_t(T1_MSB) << 16) | TCNT1;
}

void setup() {
	Serial.begin(9600);
	
	TCCR1A = 0;
	TCCR1B = (1 << CS11); // turn off the prescaler (20.14.2 pg173)
	TCCR1C = 0;
	TIMSK1 |= 0b1;
	TCNT1 = 0;
}

void loop() {
	Serial.print(get_ticks());
	Serial.print('\t');
	Serial.print(T1_MSB);
	Serial.print('\t');
	Serial.print(TCNT1);
	
	Serial.print('\n');
	delay(100);
}


ISR(INT0_vect) {
	static watch = 0;
	
	watch = TCNT1;
	
	if (watch )
}

void setup() {
	EICRA = (1 << ISC01) | (1 << ISC00); // 11 The rising edge of INT0 generates an interrupt request
	
	TCCR1A = 0;
	TCCR1B = (1 << CS10); // turn off the prescaler (20.14.2 pg173)
	TCCR1C = 0;
	
	TCNT1 = 0;
}

void loop() {

}
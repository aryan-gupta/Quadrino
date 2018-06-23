#include "iBus.h"

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

void setup() {
	Serial.begin(115200);
	setup_recv();
	setup_timer();
}

void loop() {	
	process_usart_data();

	for (uint8_t i = 0; i < 16; i++) { 
		Serial.print(recv[i]);
		Serial.print(' ');
	}
	Serial.println(' ');
	
	uint16_t s = TCNT1 + 10000;
	while (TCNT1 < s);
	
}
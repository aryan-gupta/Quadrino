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
	recv[ 0] = raw[ 0] + (raw[ 1] << 8);
	recv[ 1] = raw[ 2] + (raw[ 3] << 8);
	recv[ 2] = raw[ 4] + (raw[ 5] << 8);
	recv[ 3] = raw[ 6] + (raw[ 7] << 8);
	recv[ 4] = raw[ 8] + (raw[ 9] << 8);
	recv[ 5] = raw[10] + (raw[11] << 8);
	recv[ 6] = raw[12] + (raw[13] << 8);
	recv[ 7] = raw[14] + (raw[15] << 8);
	recv[ 8] = raw[16] + (raw[17] << 8);
	recv[ 9] = raw[18] + (raw[19] << 8);
	recv[10] = raw[20] + (raw[21] << 8);
	recv[11] = raw[22] + (raw[23] << 8); // 1  dummy channels 
	recv[12] = raw[24] + (raw[25] << 8); // 2 
	recv[13] = raw[26] + (raw[27] << 8); // 3 
	recv[14] = raw[28] + (raw[29] << 8); // 4 
	recv[15] = raw[30] + (raw[31] << 8); 

	for (uint8_t i = 0; i < 16; i++) { 
		Serial.print(recv[i]);
		Serial.print(' ');
	}
	Serial.println(' ');
	
	uint16_t s = TCNT1 + 10000;
	while (TCNT1 < s);
	
}
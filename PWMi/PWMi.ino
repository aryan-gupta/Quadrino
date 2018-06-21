// This is suplemental code for the PWM Slave
// My motors/ESC did no arrive in time but I didnt want
// to wait for them. I decided to use another slave
// arduino to 'measure' the PWM signals for the
// ESC's

#pragma once

bool ch1, ch2, ch3, ch4;
int recv_ch1, recv_ch2, recv_ch3, recv_ch4;
uint16_t timer1, timer2, timer3, timer4;


ISR(PCINT0_vect) {
	//unsigned long tmp = micros();
	uint16_t tmp = TCNT1; // this is a 16bit value
	
	// Channel 1 ========================
	if (ch1 == 0 and PINB & 0b01) {
		ch1 = 1;
		timer1 = tmp;
	} else if (ch1 == 1 and !(PINB & 0b01)) {
		ch1 = 0;
		recv_ch1 = tmp - timer1;
	}
	
	// Channel 2 ========================
	if (ch2 == 0 and PINB & 0b010) {
		ch2 = 1;
		timer2 = tmp;
	} else if (ch2 == 1 and !(PINB & 0b010)) {
		recv_ch2 = tmp - timer2;
		ch2 = 0;
	}

	// Channel 3 ========================
	if (ch3 == 0 and PINB & 0b0100) {
		ch3 = 1;
		timer3 = tmp;
	} else if (ch3 == 1 and !(PINB & 0b0100)) {
		recv_ch3 = tmp - timer3;
		ch3 = 0;
	}

	// Channel 4 ========================
	if (ch4 == 0 and PINB & 0b01000) {
		ch4 = 1;
		timer4 = tmp;
	} else if (ch4 == 1 and !(PINB & 0b01000)) {
		recv_ch4 = tmp - timer4;
		ch4 = 0;
	}
}

#ifndef FLIGHT_CTRL

void setup() {
	// There wont be any output pins (all pins default to input)
	Serial.begin(9600);
	// Reset everything to 0
	ch1 = ch2 = ch3 = ch4 = false;
	recv_ch1 = recv_ch2 = recv_ch3 = recv_ch4 = 0;
	timer1 = timer2 = timer3 = timer4 = 0;
	
	PCICR  |= (1 << PCIE0); // enable interupts on pins 7:0 (17.2.4 pg 92) 
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3); // Turn on interupts on pins 0:3 (17.2.8 pg96)

	TCCR1A = 0;
	TCCR1B = (1 << CS11); // turn off the prescaler (20.14.2 pg173)
	TCCR1C = 0;
	
	// Timer0
	TCCR0B = 0x0; // disable Timer0
	TIMSK0 = 0x0;
	TCNT0 = 0;
}

void loop() {
	Serial.print(recv_ch1);
	Serial.print('\t');
	Serial.print(recv_ch2);
	Serial.print('\t'); 
	Serial.print(recv_ch3);
	Serial.print('\t');
	Serial.print(recv_ch4);
	Serial.print('\t');
	
	
	Serial.println(" ");
	uint16_t s = TCNT1;
	while (TCNT1 < s + 10000);
}

#endif
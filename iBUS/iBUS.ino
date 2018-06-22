#include "iBus.h"

void setup() {
	setup_recv();
}

void loop() {
	auto st = micros();
	
	get_recv_data();
	
	uint32_t len = micros() - st;
	
	Serial.print(len);
	Serial.print('\t');

	for (uint8_t i = 0; i < 15; i++) { 
		Serial.print(recv[i]);
		Serial.print(' ');
	}
	Serial.println(' ');
}
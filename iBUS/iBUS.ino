#include "iBus.h"

void setup() {
	setup_recv();
}

void loop() {
	auto st = micros();
	
	rdata_idx = 0;
	UCSR0B |= (1 << RXCIE0); // Enable Rx Complete Interrupt
	while (UCSR0B & (1 << RXCIE0));
	rdata_idx = 0;
	uint32_t len = micros() - st;
	
	Serial.print(len);
	Serial.print('\t');
	
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
	recv[11] = raw[22] + (raw[23] << 8); // 1. dummy channels
	recv[12] = raw[24] + (raw[25] << 8); // 2. 
	recv[13] = raw[26] + (raw[27] << 8); // 3. 
	recv[14] = raw[28] + (raw[29] << 8); // 4. 
	recv[15] = raw[30] + (raw[31] << 8); 
	
	for (uint8_t i = 0; i < 32; i++) { 
		Serial.print(raw[i]);
		Serial.print(' ');
	}
	Serial.println(' ');
}
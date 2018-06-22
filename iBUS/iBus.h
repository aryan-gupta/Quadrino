#pragma once

uint16_t recv[15];

void setup_recv() {
	Serial.begin(115200);
}

/*
	The general iBus protocol is this:
	iBus is a Serial protocol that operates at 115.2 kHz.
	The first two bytes are sync/start bytes consisting off
	0x20 then 0x40. Then there are 14 channels of recv, each
	channel being 16bits. The recv is little endian so bytes
	must be swapped. After that, there are 2 bytes that are 
	check sum bytes and is just 0xFFFF subtracted by every byte
	we transfered. (I dont know what this means but I bet that 
	the answer is in these two links). Majority of this code is
	based on this:
	
	and this:
	
	Only 10 channels of the recv really means somthing to us so
	we extract that. 

*/

void get_recv_data() {
	uint8_t raw[30];
	
	while (true) {
		uint8_t val;
		
		while (Serial.available() < 32);
		
		val = Serial.read();
		if (val != 0x20) continue;
		
		val = Serial.read();
		if (val != 0x40) continue;
		
		for (uint8_t idx = 0; idx < 30; ++idx) {
			raw[idx] = Serial.read();
		}
		
		recv[14] = raw[28]  + (raw[29] << 8);
		
		uint16_t chksum = 0xFFFF - 0x20 - 0x40;
		for (uint8_t i = 0; i < 28; i++) {
			chksum -= raw[i];
		}
		
		if (recv[14] == chksum) break;
		else continue; // Error found in the recv
	}
	
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
	recv[10] = raw[20] + (raw[21] << 8); // 1. dummy channels
	recv[11] = raw[22] + (raw[23] << 8); // 2. 
	recv[12] = raw[24] + (raw[25] << 8); // 3. 
	recv[13] = raw[26] + (raw[27] << 8); // 4. 
}
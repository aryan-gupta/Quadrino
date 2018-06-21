#pragma once

uint16_t data[15];

void setup_recv() {
  Serial.begin(115200);
}

/*
	The general iBus protocol is this:
	iBus is a serial protocol that operates at 115.2 kHz.
	The first two bytes are sync/start bytes consisting off
	0x20 then 0x40. Then there are 14 channels of data, each
	channel being 16bits. The data is little endian so bytes
	must be swapped. After that, there are 2 bytes that are 
	check sum bytes and is just 0xFFFF subtracted by every byte
	we transfered. (I dont know what this means but I bet that 
	the answer is in these two links). Majority of this code is
	based on this:
	
	and this:
	
	Only 10 channels of the data really means somthing to us so
	we extract that. 

*/

void get_recv_data() {
	uint8_t raw[30];
	
	while (true) {
		uint8_t val;
		
		while (!Serial.available());
		val = Serial.read();
		if (val != 0x20) continue;
		
		while (!Serial.available());
		val = Serial.read();
		if (val != 0x40) continue;
		
		for (uint8_t idx = 0; idx < 30; ++idx) {
			while (!Serial.available());
			raw[idx] = Serial.read();
		}
		
		data[14] = raw[28]  + (raw[29] << 8);
		
		uint16_t chksum = 0xFFFF - 0x20 - 0x40;
		for (uint8_t i = 0; i < 28; i++) {
			chksum -= raw[i];
		}
		
		if (data[14] == chksum) break;
		else continue; // Error found in the data
	}
	
	data[ 0] = raw[ 0] + (raw[ 1] << 8);
	data[ 1] = raw[ 2] + (raw[ 3] << 8);
	data[ 2] = raw[ 4] + (raw[ 5] << 8);
	data[ 3] = raw[ 6] + (raw[ 7] << 8);
	data[ 4] = raw[ 8] + (raw[ 9] << 8);
	data[ 5] = raw[10] + (raw[11] << 8);
	data[ 6] = raw[12] + (raw[13] << 8);
	data[ 7] = raw[14] + (raw[15] << 8);
	data[ 8] = raw[16] + (raw[17] << 8);
	data[ 9] = raw[18] + (raw[19] << 8);
	data[10] = raw[20] + (raw[21] << 8); // 1. These 4 are dummy channels that we dont care about
	data[11] = raw[22] + (raw[23] << 8); // 2. 
	data[12] = raw[24] + (raw[25] << 8); // 3. 
	data[13] = raw[26] + (raw[27] << 8); // 4. 
}
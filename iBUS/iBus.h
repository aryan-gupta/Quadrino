#pragma once

uint16_t recv[16];
volatile uint8_t raw[64];
volatile uint8_t idx = 0;

// Most of this serial code is based from:
// http://forum.arduino.cc/index.php?topic=37874.0

void setup_recv() {
	unsigned long baud = 115200;
	
	uint8_t use2x = 0;
	uint16_t ubbr =  (F_CPU + 8UL * baud) / (16UL * baud) - 1UL;
	if ( (100 * (F_CPU)) > (16 * (ubbr + 1) * (100 * ubbr + ubbr * BAUD_TOL)) ) {
		use2x = 1;
		ubbr = (F_CPU + 4UL * baud) / (8UL * baud) - 1UL;
	}
	
	UBRR0L = ubbr & 0xff;
	UBRR0H = ubbr >> 8;
	if (use2x) {
		UCSR0A |= (1 << U2X0);
	} else {
		UCSR0A &= ~(1 << U2X0);
	}
	
	UCSR0B |= (1 << RXEN0); // enable recv
	UCSR0B &= ~(1 << UDRIE0); // disable Data Register Empty interrupt
	UCSR0B |= (1 << RXCIE0); // enable interrupt
}

ISR(USART_RX_vect){
	uint8_t data = UDR0;
	
	if (idx == 0 and data != 0x20) return;
	
#if USART_SAFE
	if (idx == 1 and data != 0x40) {
		idx = 0;
		return;
	}
#endif

	raw[idx] = data;
	idx = (idx + 1) % 32;
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
	https://github.com/povlhp/iBus2PPM
	and this:
	https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1/
	Only 10 channels of the recv really means somthing to us so
	we extract that. 

*/

void get_recv_data() {
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
}

uint8_t USART_Receive() {
	/* Wait for data to be received */
	while (!(UCSR0A & (1 << RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}
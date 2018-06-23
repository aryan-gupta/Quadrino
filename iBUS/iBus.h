#pragma once

const uint8_t USART_FRAME_SIZE = 32;

uint16_t recv[USART_FRAME_SIZE / 2];
volatile uint8_t raw[USART_FRAME_SIZE];
volatile uint8_t idx = 0;

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
	we extract that. The code for the Serial protocol is based on
	the code here:
	http://forum.arduino.cc/index.php?topic=37874.0
*/
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
	/// @todo maybe add a simple counter so if our checksum is
	// invalid we can be notified of new data
	// The other way we can do it is if we have a 64 byte buffer
	// while we read one buffer, the other buffer is being filled
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

void process_usart_data() {
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
	
	uint16_t chksum = 0xFFFF;
	for (uint8_t i = 0; i < 30; i++) {
		chksum -= raw[i];
	}
}
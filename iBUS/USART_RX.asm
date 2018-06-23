ISR(USART_RX_vect) {
	static uint8_t idx = 0;
	uint8_t data = UDR0;
	
	if (idx == 0 and data != 0x20) return;
	
	if (buff1State == 1) {
		usart_buffer1[idx++] = data;
		if (idx == USART_FRAME_SIZE) {
			// This will be atomic cause we are in an ISR
			buff1State = 0; // set this buffer as ready
			buff2State = 1;
			idx = 0;
		}
	} else if (buff2State == 1) {
		usart_buffer2[idx++] = data;
		if (idx == USART_FRAME_SIZE) {
			// This will be atomic cause we are in an ISR
			buff1State = 1;
			buff2State = 0; // set this buffer as ready
			idx = 0;
		}
	}
}
	
	in r2, __SREG__
	push r30
	push r31
	lds r3, 0x00C6 ; address of UDR0 register
	lds r4, %[_idx]
	cpi r4, 0x00 ; compare _idx to 0
	brne UM
	cpi r3, 0x20 ; Compare input data to 0x20 (packet start)
	brne UE
UM:	lds r5, %[_bufs1]
	cpi r5, 0x1
	breq B1
	lds r5, %[_bufs2]
	cpi r5, 0x1
	breq B2
B1:	lds r31, %A[_buf1] ; load base pointer
	lds r30, %B[_buf1] ; http://www.avrbeginners.net/assembler/macros.html (maybe)
	ldi r5, 0x0
	add r30, r4 ; add idx
	adc r31, r5
	st Z, r3
	inc r4
	cpi r4, 0x20
	brne IE
	sts %[_bufs1], r5
	sts %[_idx], r5
	ldi r5, 0x1
	sts %[_bufs2], r5
	rjmp UE
B2:	lds r31, %A[_buf2] ; load base pointer
	lds r30, %B[_buf2]
	ldi r5, 0x0
	add r30, r4 ; add idx
	adc r31, r5
	st Z, r3
	inc r4
	cpi r4, 0x20
	brne IE
	sts %[_bufs2], r5
	sts %[_idx], r5
	ldi r5, 0x1
	sts %[_bufs1], r5
	rjmp UE
IE: sts %[_idx], r4
UE:	pop r31
	pop r30
	out __SREG__, r2
	reti
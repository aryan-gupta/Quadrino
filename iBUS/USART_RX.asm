ISR(USART_RX_vect) {
	static uint8_t* loc = buffer_start;
	uint8_t data = UDR0;
	
	if (loc == buffer_start or loc == buffer2_start) {
		if (data != 0x20) return;
	}
	
	*loc = data;
	loc++;
	
	if (loc == buffer2_start) { // technically we only need to compare the LSB because its an array
		buff1_ready = true;
		buff2_ready = false;
		return;
	}
	
	if (loc == buffer_end) {
		buff2_ready = true;
		buff1_ready = false;
		loc = buffer_start;
		return;
	}
}


; Use r31 as r4 until we upload the data
; then load _loc MSB into r31
	in r2, __SREG__
	push r30
	push r31
	lds r4, 0x00C6 ; address of UDR0 register
	lds r30, %B[_loc] ; load loc LSB, we are going to load MSB of loc later
	cpi r30, %B[_buf1s] ; Compare loc LSB with buffer_start
	breq SB ; TODO goto check for 0x20 (start bit)
	cpi r30, %B[_buf2s]
	brne MN ; TODO goto main
SB:	ldi r31, 0x20
	cp r4, r31
	brne EN ; TODO goto end
MN:	lds r31, %A[_loc] ; MSB
	st Z+, r4
	cpi r30, %B[_buf2s]
	brne L2 ; branch to check buffer 2
	sts %B[_loc], r30
	sts %A[_loc], r31
	ldi r30, 0x0
	ldi r31, 0x1
	sts %[buff1r], r31
	sts %[buff2r], r30
	rjmp EN
L2: cpi r30, %B[_bufe]
	brne EN
	ldi r30, %B[_buf1s]
	ldi r31, %A[_buf1s]
	sts %B[_loc], r30
	sts %A[_loc], r31
	ldi r30, 0x0
	ldi r31, 0x1
	sts %[buff1r], r30
	sts %[buff2r], r31
EN: pop r31
	pop r30
	out __SREG__, r2
in __temp_reg__, __SREG__
eor __zero_reg__, __zero_reg__
push r18
push r19
push r20
push r21
lds r18, 0x84
lds r19, 0x85
C1:	sbis 0x23, 0 ; if first bit of PORTD is high got E1
	rjmp E1
	lds r20, %[_c1] ; if ch1 == 1 goto CHANNEL2
	cpi r20, 0x0 ; test if ch1 is 0
	brne C2 ; false (goto channel 2)
	ldi r20, 0x1 ; ch1 = 1
	sts %[_c1], r20 ; true
	sts	%A[_t1], r18 ; timer1 = TCNT1
	sts	%B[_t1], r19
	rjmp C2 ; goto channel 2
E1:	lds r20, %[_c1] ; r20 = ch1
	cpi r20, 0x1 ; if ch1 != 1 gpto CHANNEL2
	brne C2
	sts %[_c1], __zero_reg__ ; ch1 = 0
	lds r20, %A[_t1] ; recv_ch1 = TCNT1 - timer1
	lds r21, %B[_t1]
	sub r20, r18
	sbc r21, r19
	sts %A[_r1], r20
	sts %B[_r1], r21
C2:
pop r21
pop r20
pop r19
pop r18
out __SREG__, __temp_reg__
reti
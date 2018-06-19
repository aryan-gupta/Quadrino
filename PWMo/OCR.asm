; asm volatile (

	push r24
	in r24, __SREG__
	push r24
	push r25

	in	r25, PORTD
	lds	r24, %0
	and	r24, r25
	out	PORTD, r24

	in r24, TIMSK1
	andi r24, 0xFB ; 0b11111011
	out TIMSK1, r24

	pop r25
	pop r24
	out __SREG__, r24
	pop r24
	reti

;	:  // Outputs
;	: "m" (escBDown) // Inputs
;	: r24, r25 // Clobber list

; )


; %0 is escBDown

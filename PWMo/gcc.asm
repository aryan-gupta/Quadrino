
	PORTD &= escBDown;
	TIMSK1 &= 0b11111011;


in	r25, 0x0b
lds	r24, 0x01B8
and	r24, r25
out	0x0b, r24
ldi	r30, 0x6F
ldi	r31, 0x00
ld	r24, Z
andi	r24, 0xFB
st	Z, r24



push	r1
push	r0
in	r0, 0x3f	; 63
push	r0
eor	r1, r1
push	r24
push	r25
push	r30
push	r31
in	r25, 0x0b	; 11
lds	r24, 0x01B8	; 0x8001b8 <escBDown>
and	r24, r25
out	0x0b, r24	; 11
ldi	r30, 0x6F	; 111
ldi	r31, 0x00	; 0
ld	r24, Z
andi	r24, 0xFB	; 251
st	Z, r24
pop	r31
pop	r30
pop	r25
pop	r24
pop	r0
out	0x3f, r0	; 63
pop	r0
pop	r1
reti



 5dc:	8f 93       	push	r24
 5de:	8f b7       	in	r24, 0x3f	; 63
 5e0:	8f 93       	push	r24
 5e2:	9f 93       	push	r25
 5e4:	9b b1       	in	r25, 0x0b	; 11
 5e6:	80 91 b8 01 	lds	r24, 0x01B8	; 0x8001b8 <escBDown>
 5ea:	89 23       	and	r24, r25
 5ec:	8b b9       	out	0x0b, r24	; 11
 5ee:	80 81       	ld	r24, Z
 5f0:	8b 7f       	andi	r24, 0xFB	; 251
 5f2:	80 83       	st	Z, r24
 5f4:	9f 91       	pop	r25
 5f6:	8f 91       	pop	r24
 5f8:	8f bf       	out	0x3f, r24	; 63
 5fa:	8f 91       	pop	r24
 5fc:	18 95       	reti
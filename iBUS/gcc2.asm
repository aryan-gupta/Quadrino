310:	1f 92       	push	r1
 312:	0f 92       	push	r0
 314:	0f b6       	in	r0, 0x3f	; 63
 316:	0f 92       	push	r0
 318:	11 24       	eor	r1, r1
 31a:	8f 93       	push	r24
 31c:	9f 93       	push	r25
 31e:	ef 93       	push	r30
 320:	ff 93       	push	r31
 322:	80 91 c6 00 	lds	r24, 0x00C6	; 0x8000c6 <__TEXT_REGION_LENGTH__+0x7e00c6>
 326:	e0 91 00 01 	lds	r30, 0x0100	; 0x800100 <__data_start>
 32a:	f0 91 01 01 	lds	r31, 0x0101	; 0x800101 <__data_start+0x1>
 32e:	91 e0       	ldi	r25, 0x01	; 1
 330:	e4 31       	cpi	r30, 0x14	; 20
 332:	f9 07       	cpc	r31, r25
 334:	21 f0       	breq	.+8      	; 0x33e <__vector_18+0x2e>
 336:	91 e0       	ldi	r25, 0x01	; 1
 338:	e4 33       	cpi	r30, 0x34	; 52
 33a:	f9 07       	cpc	r31, r25
 33c:	11 f4       	brne	.+4      	; 0x342 <__vector_18+0x32>
 33e:	80 32       	cpi	r24, 0x20	; 32
 340:	e9 f4       	brne	.+58     	; 0x37c <__vector_18+0x6c>
 342:	81 93       	st	Z+, r24
 344:	f0 93 01 01 	sts	0x0101, r31	; 0x800101 <__data_start+0x1>
 348:	e0 93 00 01 	sts	0x0100, r30	; 0x800100 <__data_start>
 34c:	81 e0       	ldi	r24, 0x01	; 1
 34e:	e4 33       	cpi	r30, 0x34	; 52
 350:	f8 07       	cpc	r31, r24
 352:	31 f4       	brne	.+12     	; 0x360 <__vector_18+0x50>
 354:	81 e0       	ldi	r24, 0x01	; 1
 356:	80 93 13 01 	sts	0x0113, r24	; 0x800113 <buff1_ready>
 35a:	10 92 12 01 	sts	0x0112, r1	; 0x800112 <__data_end>
 35e:	0e c0       	rjmp	.+28     	; 0x37c <__vector_18+0x6c>
 360:	e4 55       	subi	r30, 0x54	; 84
 362:	f1 40       	sbci	r31, 0x01	; 1
 364:	59 f4       	brne	.+22     	; 0x37c <__vector_18+0x6c>
 366:	81 e0       	ldi	r24, 0x01	; 1
 368:	80 93 12 01 	sts	0x0112, r24	; 0x800112 <__data_end>
 36c:	10 92 13 01 	sts	0x0113, r1	; 0x800113 <buff1_ready>
 370:	84 e1       	ldi	r24, 0x14	; 20
 372:	91 e0       	ldi	r25, 0x01	; 1
 374:	90 93 01 01 	sts	0x0101, r25	; 0x800101 <__data_start+0x1>
 378:	80 93 00 01 	sts	0x0100, r24	; 0x800100 <__data_start>
 37c:	ff 91       	pop	r31
 37e:	ef 91       	pop	r30
 380:	9f 91       	pop	r25
 382:	8f 91       	pop	r24
 384:	0f 90       	pop	r0
 386:	0f be       	out	0x3f, r0	; 63
 388:	0f 90       	pop	r0
 38a:	1f 90       	pop	r1
 38c:	18 95       	reti
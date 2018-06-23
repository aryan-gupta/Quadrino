 310:	1f 92       	push	r1
 312:	0f 92       	push	r0
 314:	0f b6       	in	r0, 0x3f	; 63
 316:	0f 92       	push	r0
 318:	11 24       	eor	r1, r1
 31a:	2f 93       	push	r18
 31c:	8f 93       	push	r24
 31e:	9f 93       	push	r25
 320:	ef 93       	push	r30
 322:	ff 93       	push	r31
 324:	80 91 c6 00 	lds	r24, 0x00C6	; 0x8000c6 <__TEXT_REGION_LENGTH__+0x7e00c6>
 328:	e0 91 10 01 	lds	r30, 0x0110	; 0x800110 <__data_end>
 32c:	e1 11       	cpse	r30, r1
 32e:	02 c0       	rjmp	.+4      	; 0x334 <__vector_18+0x24>
 330:	80 32       	cpi	r24, 0x20	; 32
 332:	39 f5       	brne	.+78     	; 0x382 <__vector_18+0x72>
 334:	90 91 00 01 	lds	r25, 0x0100	; 0x800100 <__data_start>
 338:	91 30       	cpi	r25, 0x01	; 1
 33a:	79 f4       	brne	.+30     	; 0x35a <__vector_18+0x4a>
 33c:	21 e0       	ldi	r18, 0x01	; 1
 33e:	2e 0f       	add	r18, r30
 340:	20 93 10 01 	sts	0x0110, r18	; 0x800110 <__data_end>
 344:	f0 e0       	ldi	r31, 0x00	; 0
 346:	eb 57       	subi	r30, 0x7B	; 123
 348:	fe 4f       	sbci	r31, 0xFE	; 254
 34a:	80 83       	st	Z, r24
 34c:	20 32       	cpi	r18, 0x20	; 32
 34e:	c9 f4       	brne	.+50     	; 0x382 <__vector_18+0x72>
 350:	10 92 00 01 	sts	0x0100, r1	; 0x800100 <__data_start>
 354:	90 93 a5 01 	sts	0x01A5, r25	; 0x8001a5 <buff2State>
 358:	12 c0       	rjmp	.+36     	; 0x37e <__vector_18+0x6e>
 35a:	90 91 a5 01 	lds	r25, 0x01A5	; 0x8001a5 <buff2State>
 35e:	91 30       	cpi	r25, 0x01	; 1
 360:	81 f4       	brne	.+32     	; 0x382 <__vector_18+0x72>
 362:	21 e0       	ldi	r18, 0x01	; 1
 364:	2e 0f       	add	r18, r30
 366:	20 93 10 01 	sts	0x0110, r18	; 0x800110 <__data_end>
 36a:	f0 e0       	ldi	r31, 0x00	; 0
 36c:	eb 59       	subi	r30, 0x9B	; 155
 36e:	fe 4f       	sbci	r31, 0xFE	; 254
 370:	80 83       	st	Z, r24
 372:	20 32       	cpi	r18, 0x20	; 32
 374:	31 f4       	brne	.+12     	; 0x382 <__vector_18+0x72>
 376:	90 93 00 01 	sts	0x0100, r25	; 0x800100 <__data_start>
 37a:	10 92 a5 01 	sts	0x01A5, r1	; 0x8001a5 <buff2State>
 37e:	10 92 10 01 	sts	0x0110, r1	; 0x800110 <__data_end>
 382:	ff 91       	pop	r31
 384:	ef 91       	pop	r30
 386:	9f 91       	pop	r25
 388:	8f 91       	pop	r24
 38a:	2f 91       	pop	r18
 38c:	0f 90       	pop	r0
 38e:	0f be       	out	0x3f, r0	; 63
 390:	0f 90       	pop	r0
 392:	1f 90       	pop	r1
 394:	18 95       	reti
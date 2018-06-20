00000584 <__vector_3>:
 584:	1f 92       	push	r1
 586:	0f 92       	push	r0
 588:	0f b6       	in	r0, 0x3f	; 63
 58a:	0f 92       	push	r0
 58c:	11 24       	eor	r1, r1
 58e:	2f 93       	push	r18
 590:	3f 93       	push	r19
 592:	4f 93       	push	r20
 594:	5f 93       	push	r21
 596:	8f 93       	push	r24
 598:	9f 93       	push	r25
 59a:	80 91 84 00 	lds	r24, 0x0084	; 0x800084 <__TEXT_REGION_LENGTH__+0x7e0084>
 59e:	90 91 85 00 	lds	r25, 0x0085	; 0x800085 <__TEXT_REGION_LENGTH__+0x7e0085>
 5a2:	20 91 29 01 	lds	r18, 0x0129	; 0x800129 <ch1>
 5a6:	21 11       	cpse	r18, r1
 5a8:	03 c0       	rjmp	.+6      	; 0x5b0 <__vector_3+0x2c>
 5aa:	18 99       	sbic	0x03, 0	; 3
 5ac:	32 c0       	rjmp	.+100    	; 0x612 <__vector_3+0x8e>
 5ae:	02 c0       	rjmp	.+4      	; 0x5b4 <__vector_3+0x30>
 5b0:	18 9b       	sbis	0x03, 0	; 3
 5b2:	37 c0       	rjmp	.+110    	; 0x622 <__vector_3+0x9e>
 5b4:	20 91 2a 01 	lds	r18, 0x012A	; 0x80012a <ch2>
 5b8:	21 11       	cpse	r18, r1
 5ba:	03 c0       	rjmp	.+6      	; 0x5c2 <__vector_3+0x3e>
 5bc:	19 99       	sbic	0x03, 1	; 3
 5be:	3f c0       	rjmp	.+126    	; 0x63e <__vector_3+0xba>
 5c0:	02 c0       	rjmp	.+4      	; 0x5c6 <__vector_3+0x42>
 5c2:	19 9b       	sbis	0x03, 1	; 3
 5c4:	44 c0       	rjmp	.+136    	; 0x64e <__vector_3+0xca>
 5c6:	20 91 2b 01 	lds	r18, 0x012B	; 0x80012b <ch3>
 5ca:	21 11       	cpse	r18, r1
 5cc:	03 c0       	rjmp	.+6      	; 0x5d4 <__vector_3+0x50>
 5ce:	1a 99       	sbic	0x03, 2	; 3
 5d0:	4c c0       	rjmp	.+152    	; 0x66a <__vector_3+0xe6>
 5d2:	02 c0       	rjmp	.+4      	; 0x5d8 <__vector_3+0x54>
 5d4:	1a 9b       	sbis	0x03, 2	; 3
 5d6:	51 c0       	rjmp	.+162    	; 0x67a <__vector_3+0xf6>
 5d8:	20 91 2c 01 	lds	r18, 0x012C	; 0x80012c <ch4>
 5dc:	21 11       	cpse	r18, r1
 5de:	0a c0       	rjmp	.+20     	; 0x5f4 <__vector_3+0x70>
 5e0:	1b 9b       	sbis	0x03, 3	; 3
 5e2:	59 c0       	rjmp	.+178    	; 0x696 <__vector_3+0x112>
 5e4:	21 e0       	ldi	r18, 0x01	; 1
 5e6:	20 93 2c 01 	sts	0x012C, r18	; 0x80012c <ch4>
 5ea:	90 93 28 01 	sts	0x0128, r25	; 0x800128 <timer4+0x1>
 5ee:	80 93 27 01 	sts	0x0127, r24	; 0x800127 <timer4>
 5f2:	51 c0       	rjmp	.+162    	; 0x696 <__vector_3+0x112>
 5f4:	1b 99       	sbic	0x03, 3	; 3
 5f6:	4f c0       	rjmp	.+158    	; 0x696 <__vector_3+0x112>
 5f8:	20 91 27 01 	lds	r18, 0x0127	; 0x800127 <timer4>
 5fc:	30 91 28 01 	lds	r19, 0x0128	; 0x800128 <timer4+0x1>
 600:	82 1b       	sub	r24, r18
 602:	93 0b       	sbc	r25, r19
 604:	90 93 2e 01 	sts	0x012E, r25	; 0x80012e <recv_ch4+0x1>
 608:	80 93 2d 01 	sts	0x012D, r24	; 0x80012d <recv_ch4>
 60c:	10 92 2c 01 	sts	0x012C, r1	; 0x80012c <ch4>
 610:	42 c0       	rjmp	.+132    	; 0x696 <__vector_3+0x112>
 612:	21 e0       	ldi	r18, 0x01	; 1
 614:	20 93 29 01 	sts	0x0129, r18	; 0x800129 <ch1>
 618:	90 93 22 01 	sts	0x0122, r25	; 0x800122 <timer1+0x1>
 61c:	80 93 21 01 	sts	0x0121, r24	; 0x800121 <timer1>
 620:	c9 cf       	rjmp	.-110    	; 0x5b4 <__vector_3+0x30>
 622:	10 92 29 01 	sts	0x0129, r1	; 0x800129 <ch1>
 626:	20 91 21 01 	lds	r18, 0x0121	; 0x800121 <timer1>
 62a:	30 91 22 01 	lds	r19, 0x0122	; 0x800122 <timer1+0x1>
 62e:	ac 01       	movw	r20, r24
 630:	42 1b       	sub	r20, r18
 632:	53 0b       	sbc	r21, r19
 634:	50 93 34 01 	sts	0x0134, r21	; 0x800134 <recv_ch1+0x1>
 638:	40 93 33 01 	sts	0x0133, r20	; 0x800133 <recv_ch1>
 63c:	bb cf       	rjmp	.-138    	; 0x5b4 <__vector_3+0x30>
 63e:	21 e0       	ldi	r18, 0x01	; 1
 640:	20 93 2a 01 	sts	0x012A, r18	; 0x80012a <ch2>
 644:	90 93 24 01 	sts	0x0124, r25	; 0x800124 <timer2+0x1>
 648:	80 93 23 01 	sts	0x0123, r24	; 0x800123 <timer2>
 64c:	bc cf       	rjmp	.-136    	; 0x5c6 <__vector_3+0x42>
 64e:	20 91 23 01 	lds	r18, 0x0123	; 0x800123 <timer2>
 652:	30 91 24 01 	lds	r19, 0x0124	; 0x800124 <timer2+0x1>
 656:	ac 01       	movw	r20, r24
 658:	42 1b       	sub	r20, r18
 65a:	53 0b       	sbc	r21, r19
 65c:	50 93 32 01 	sts	0x0132, r21	; 0x800132 <recv_ch2+0x1>
 660:	40 93 31 01 	sts	0x0131, r20	; 0x800131 <recv_ch2>
 664:	10 92 2a 01 	sts	0x012A, r1	; 0x80012a <ch2>
 668:	ae cf       	rjmp	.-164    	; 0x5c6 <__vector_3+0x42>
 66a:	21 e0       	ldi	r18, 0x01	; 1
 66c:	20 93 2b 01 	sts	0x012B, r18	; 0x80012b <ch3>
 670:	90 93 26 01 	sts	0x0126, r25	; 0x800126 <timer3+0x1>
 674:	80 93 25 01 	sts	0x0125, r24	; 0x800125 <timer3>
 678:	af cf       	rjmp	.-162    	; 0x5d8 <__vector_3+0x54>
 67a:	20 91 25 01 	lds	r18, 0x0125	; 0x800125 <timer3>
 67e:	30 91 26 01 	lds	r19, 0x0126	; 0x800126 <timer3+0x1>
 682:	ac 01       	movw	r20, r24
 684:	42 1b       	sub	r20, r18
 686:	53 0b       	sbc	r21, r19
 688:	50 93 30 01 	sts	0x0130, r21	; 0x800130 <recv_ch3+0x1>
 68c:	40 93 2f 01 	sts	0x012F, r20	; 0x80012f <recv_ch3>
 690:	10 92 2b 01 	sts	0x012B, r1	; 0x80012b <ch3>
 694:	a1 cf       	rjmp	.-190    	; 0x5d8 <__vector_3+0x54>
 696:	9f 91       	pop	r25
 698:	8f 91       	pop	r24
 69a:	5f 91       	pop	r21
 69c:	4f 91       	pop	r20
 69e:	3f 91       	pop	r19
 6a0:	2f 91       	pop	r18
 6a2:	0f 90       	pop	r0
 6a4:	0f be       	out	0x3f, r0	; 63
 6a6:	0f 90       	pop	r0
 6a8:	1f 90       	pop	r1
 6aa:	18 95       	reti
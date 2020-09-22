.global a, c
.extern b
.equ g, 7
.text
	jeqw	$a
	jeqw	$e
	jeqw 	b
	jeqw	$d
	addw	r1h[a], [r5l]
d: 	.word	  d
	movw 	r0, b
	movw	c, r0
	movw	r0, e
.data
.skip 	8
e:	.word	a
	.word	e
	.word	.bss 
a:	.word	b
.bss
c:	.skip 	8
.end


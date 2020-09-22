.equ g,4
.text
movw r1,&g
orb r1h[a],r1
orw r1h[255],r1
addw r1,r2
addb r1h,r2h
addb r1h,255
subw r1,*255
andw r1,[r2]
andb r1h,[r2h]
jmpw $a

ret


.data
.skip 5
a:
.skip 5
.end

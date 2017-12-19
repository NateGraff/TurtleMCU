.org 0x000

start:	mvl r1, 0x12
		mvl r2, 0x01
		call myf
		out r1

done:	jmp done # loop forever

.org 0x100
# function to add r1, r2
myf:	add r1, r2
		ret

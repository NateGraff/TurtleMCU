.org 0x000

		.ldtag r0, s1
		call puts
		mvh  r0, 0x02	# memory address 0x200
		call gets
		call puts

done:	jmp done

.org 0x100
.string s1 "Type some stuff: "

.org 0x300

# getc
# r0 - memory location to place the charater
getc:	push r1			# save the destination reg
		in   r1			# read in a word
		st   r1, [r0]	# store the character
		pop r1			# restore the reg
		ret

# putc
# r0 - memory location of character to place
putc:	push r1
		push r2

		clr  r2
		mvl  r2, 0x02   # output code for text
		out  r2			# write output code

		ld   r1, [r0]	# load the char
		out  r1			# output the char

		clr  r2			# create null byte
		out  r2

		pop  r2
		pop  r1
		ret

# gets
# r0 - memory location to place the string
gets:	push r0
		push r1
		push r2
		push r3

		clr  r1
		mvl  r1, 0x0a	# newline
		clr  r3
		mvl  r3, 0x01   # 1

loopgets: call getc
		ld   r2, [r0]
		add  r0, r3		# increment by 1
		cmp  r1, r2		# did we get a newline?
		jnz  loopgets
		clr  r2
		st   r2, [r0]	# append null byte

		pop  r3
		pop  r2
		pop  r1
		pop  r0
		ret

# puts
# r0 - memory location of string
puts:	push r0
		push r1
		push r2
		push r3

		clr  r2
		mvl  r2, 0x01	# 1
		clr  r3			# 0

loopputs: call putc
		ld   r1, [r0]
		add  r0, r2		# increment by 1
		cmp  r1, r3		# was the last character a null byte?
		jnz  loopputs

		pop  r3
		pop  r2
		pop  r1
		pop  r0
		ret

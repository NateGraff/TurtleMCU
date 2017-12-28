.org 0x000

start:	lsp  0x80
		.ldtag r0, s1
		call puts
		rsp  r0
		mvl  r1, 0x20
		sub  r0, r1
		call gets
		call puts
done:	jmp done

.org 0x100
.string s1 "Please type something: "
.string s2 "This should never happen!"

.org 0x200
		.ldtag r0, s2
		call puts
		jmp done

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

loopgets: in r2			# read in a char
		st   r2, [r0]	# store the character
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

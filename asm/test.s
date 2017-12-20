# TurtleMCU
# Full CPU Test Program

.org 0x000

# r0 - current test status
# r1 - bitmask for current test
		mvl r1, 0x01
		mvl r2, 0x01

# Test 1a
		mvh r3, 0x55
		mv  r4, r3
		cmp r3, r4
		jnz done1a
		or r0, r1
done1a:	shl r1, r2
		clr r3
		clr r4

# Test 1b
		mvl r4, 0x01
		mvl r5, 0x02
		add r3, r4
		add r3, r4
		cmp r3, r5
		jnz done1b
		or  r0, r1
done1b:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1c
		mvl r5, 0x02
		mvl r4, 0x01
		sub r5, r4
		sub r5, r4
		cmp r3, r5
		jnz done1c
		or  r0, r1
done1c:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1d
		mvl r3, 0x5F
		mvl r4, 0xAF
		mvl r5, 0x0F
		and r3, r4
		cmp r3, r5
		jnz done1d
		or  r0, r1
done1d:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1e
		mvl r3, 0x5F
		mvl r4, 0xAF
		mvl r5, 0xFF
		or  r3, r4
		cmp r3, r5
		jnz done1e
		or  r0, r1
done1e:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1f
		mvl r3, 0x5F
		mvl r4, 0xAF
		mvl r5, 0xF0
		xor r3, r4
		cmp r3, r5
		jnz done1e
		or  r0, r1
done1f:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1g
		mvh r3, 0xFF
		mvl r3, 0xFF
		not r3
		cmp r3, r4
		jnz done1g
		or  r0, r1
done1g:	shl r1, r2

# Test 1h
		mvh r3, 0xFF
		mvl r4, 0x08
		mvl r5, 0xFF
		shr r3, r4
		cmp r3, r5
		jnz done1h
		or  r0, r1
done1h:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1i
		lsp 0x300
		rsp r3
		mvl r4, 0x55
		mvl r5, 0xAA
		st  r4, [sp+1]
		st  r5, [r3-1]
		ld  r6, [sp-1]
		ld  r7, [r3+1]
		cmp r4, r7
		jnz done1i
		cmp r5, r6
		jnz done1i
		or  r0, r1
done1i:	shl r1, r2
		clr r3
		clr r4
		clr r5
		clr r6
		clr r7

# Test 1j
		mvh r3, 0x03
		lsp r3
		mvl r4, 0x55
		push r4
		ld  r5, [r3]
		cmp r4, r5
		jnz done1j
		or  r0, r1
done1j:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1k
		mvh r3, 0x03
		lsp r3
		mvl r4, 0xAA
		st  r4, [r3+1]
		pop r5
		cmp r4, r5
		jnz done1k
		or  r0, r1
done1k:	shl r1, r2
		clr r3
		clr r4
		clr r5

# Test 1l
		mvh r3, 0xFF
		mvl r3, 0xFF
		mvl r4, 0x01
		add r3, r4
		jnc done1l
		or  r0, r1
done1l: shl r1, r2
		clr r3
		clr r4

# Test 1m
		mvl r4, 0x01
		sub r3, r4
		jnc done1m
		or  r0, r1
done1m: shl r1, r2
		clr r3
		clr r4

# Test 1n
		mvl r3, 0x05
		mvl r4, 0x0a
		sub r3, r4
		jnc done1n
		or  r0, r1
done1n:	shl r1, r2
		clr r3
		clr r4

# Test 1o
		mvl r3, 0x05
		mvl r4, 0x04
		sub r3, r4 # 5 - 4 = 1
		jc  done1o
		clr r3
		clr r4

		mvl r3, 0x05
		mvl r4, 0x04
		sub r4, r3 # 4 - 5 = -1
		jnc done1o
		clr r3
		clr r4

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r4 # r5 = -4
		sub r3, r5 # 5 - (-4) = 9
		jc  done1o
		clr r3
		clr r4
		clr r5

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r3 # r5 = -5
		sub r4, r5 # 4 - (-5) = 9
		jc done1o
		clr r3
		clr r4
		clr r5

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r4 # r5 = -4
		sub r5, r4 # (-4) - 5 = -9
		jnc  done1o
		clr r3
		clr r4
		clr r5

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r3 # r5 = -5
		sub r5, r4 # (-5) - 4 = -9
		jnc done1o
		clr r3
		clr r4
		clr r5

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r3 # r5 = -5
		sub r6, r4 # r6 = -4
		sub r5, r6 # (-5) - (-4) = -1
		jnc done1o
		clr r3
		clr r4
		clr r5
		clr r6

		mvl r3, 0x05
		mvl r4, 0x04
		sub r5, r3 # r5 = -5
		sub r6, r4 # r6 = -4
		sub r6, r5 # (-4) - (-5) = -1
		jc done1o
		clr r3
		clr r4
		clr r5
		clr r6

		or  r0, r1
done1o:	shl r1, r2
		clr r3
		clr r4
		clr r5
		clr r6

# Test 1p
		mvl r3, 0x01
		mvl r4, 0x02
		mvl r5, 0x03
		call func1p
		cmp r3, r5
		jnz done1p
		or r0, r1
done1p:	shl r1, r2
		clr r3
		clr r4
		clr r5
		
		clr r1
		mvl r1, 0x01
		out r1
		out r0
		clr r0

done:	jmp done


.org 0x200

func1p:	add r3, r4
		ret

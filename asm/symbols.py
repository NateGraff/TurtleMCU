
register_defines = {
	'r0': '`R0',
	'r1': '`R1',
	'r2': '`R2',
	'r3': '`R3',
	'r4': '`R4',
	'r5': '`R5',
	'r6': '`R6',
	'r7': '`R7',
}
oponly_defines = {
	'nop': '`OPCODE_NOP',
	'ret': '`OPCODE_RET',
}
reg_defines = {
	'push': '`OPCODE_PUSH',
	'pop': '`OPCODE_POP',
	'in': '`OPCODE_IN',
	'out': '`OPCODE_OUT',
	'clr': '`OPCODE_CLR',
	'lsp': '`OPCODE_LSPR',
	'rsp': '`OPCODE_RSP',
	'not': '`OPCODE_NOT',
}
regimm_defines = {
	'mvh': '`OPCODE_MVH',
	'mvl': '`OPCODE_MVL',
}
regoffset_defines = {
	'ld': '`OPCODE_LD',
	'st': '`OPCODE_ST',
}
regreg_defines = {
	'mv': '`OPCODE_MV',
	'cmp': '`OPCODE_CMP',
	'add': '`OPCODE_ADD',
	'sub': '`OPCODE_SUB',
	'and': '`OPCODE_AND',
	'or': '`OPCODE_OR',
	'xor': '`OPCODE_XOR',
	'shr': '`OPCODE_SHR',
	'shl': '`OPCODE_SHL',
}
regregoffset_defines = {
	'ld': '`OPCODE_LDI',
	'st': '`OPCODE_STI',
}
tag_defines = {
	'call': '`OPCODE_CALL',
	'jmp': '`OPCODE_JMP',
	'jc': '`OPCODE_JC',
	'jnc': '`OPCODE_JNC',
	'jz': '`OPCODE_JZ',
	'jnz': '`OPCODE_JNZ',
}
addr_defines = {
	'lsp': '`OPCODE_LSPA',
}

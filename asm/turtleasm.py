#!/usr/bin/env python3

from pyparsing import * 

comment = '#' + restOfLine
tag = Word(alphanums).setResultsName('tag') + ':'
tagref = Word(alphanums)
register = Or([CaselessKeyword(k) for k in ['r0','r1','r2','r3','r4','r5','r6','r7']])
sp = CaselessKeyword('sp')
immediate = Combine('0x' + Word(alphanums, max=2))
address = Combine('0x' + Word(alphanums, max=3))
regaddr = '[' + register.setResultsName('src') + Optional(Combine(Or(['+','-']) + Word(nums))).setResultsName('offset') + ']'
spaddr = '[' + sp + Optional(Combine(Or(['+','-']) + Word(nums))).setResultsName('offset') + ']'

org = CaselessKeyword('.org') + White() + address.setResultsName('address')

def setTypeIgnore(toks):
	toks['type'] = 'ignore'
	return toks
def setTypeDirective(toks):
	toks['type'] = 'directive'
	return toks
def setTypeOpOnly(toks):
	toks['type'] = 'oponly'
	return toks
def setTypeReg(toks):
	toks['type'] = 'reg'
	return toks
def setTypeRegImm(toks):
	toks['type'] = 'regimm'
	return toks
def setTypeRegOffset(toks):
	toks['type'] = 'regoffset'
	return toks
def setTypeRegReg(toks):
	toks['type'] = 'regreg'
	return toks
def setTypeRegRegOffset(toks):
	toks['type'] = 'regregoffset'
	return toks
def setTypeTag(toks):
	toks['type'] = 'tag'
	return toks
def setTypeAddr(toks):
	toks['type'] = 'addr'
	return toks

opcode_oponly = Or([CaselessKeyword(k) for k in ['nop','ret']]).setParseAction(setTypeOpOnly)
opcode_reg = Or([CaselessKeyword(k) for k in ['push','pop','in','out','clr','lsp','rsp','not']]).setParseAction(setTypeReg)
opcode_regimm = Or([CaselessKeyword(k) for k in ['mvh','mvl']]).setParseAction(setTypeRegImm)
opcode_regoffset = Or([CaselessKeyword(k) for k in ['ld','st']]).setParseAction(setTypeRegOffset)
opcode_regreg = Or([CaselessKeyword(k) for k in ['mv','cmp','add','sub','and','or','xor','shr','shl']]).setParseAction(setTypeRegReg)
opcode_regregoffset = Or([CaselessKeyword(k) for k in ['ld','st']]).setParseAction(setTypeRegRegOffset)
opcode_tag = Or([CaselessKeyword(k) for k in ['call','jmp','jc','jnc','jz','jnz']]).setParseAction(setTypeTag)
opcode_addr = Or([CaselessKeyword(k) for k in ['lsp']]).setParseAction(setTypeAddr)

instruction_oponly = opcode_oponly.setResultsName('opcode')
instruction_reg = opcode_reg.setResultsName('opcode') + White() + register.setResultsName('dest')
instruction_regimm = opcode_regimm.setResultsName('opcode') + White() + register.setResultsName('dest') + Optional(White()) + ',' + Optional(White()) + immediate.setResultsName('src')
instruction_regoffset = opcode_regoffset.setResultsName('opcode') + White() + register.setResultsName('dest') + Optional(White()) + ',' + Optional(White()) + spaddr
instruction_regreg = opcode_regreg.setResultsName('opcode') + White() + register.setResultsName('dest') + Optional(White()) + ',' + Optional(White()) + register.setResultsName('src')
instruction_regregoffset = opcode_regregoffset.setResultsName('opcode') + White() + register.setResultsName('dest') + Optional(White()) + ',' + Optional(White()) + regaddr
instruction_tag = opcode_tag.setResultsName('opcode') + White() + tagref.setResultsName('tagref')
instruction_addr = opcode_addr.setResultsName('opcode') + White() + address.setResultsName('address')
instruction = Or([instruction_oponly,instruction_reg,instruction_regimm,instruction_regoffset,instruction_regreg,instruction_regregoffset,instruction_tag,instruction_addr])

blankline = (White() + LineEnd()).setParseAction(setTypeIgnore)
commentline = (Optional(White()) + comment).setParseAction(setTypeIgnore)
untaggedline = Optional(White()) + instruction + Optional(White()) + Or([comment, LineEnd()])
taggedline = tag + untaggedline
directiveline = (Or([org]).setResultsName('directive') + Optional(White()) + LineEnd()).setParseAction(setTypeDirective)
line = Or([blankline,commentline,untaggedline,taggedline,directiveline])

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

def parseLine(text):
	try:
		return line.parseString(text).asDict()
	except ParseException as e:
		print("Parse Error:")
		print(e.line)
		print(" " * (e.col - 1) + "^")

if __name__ == "__main__":
	import sys
	if(len(sys.argv) < 2):
		print("Please supply a file to assemble.")
	else:
		with open(sys.argv[1], 'r') as source:
			with open("rom.sv", 'w') as rom:
				rom.write("""
`include "constants.sv"
module rom(
	input wire [9:0] addr,
	output wire [15:0] dout);

	always_comb begin
		case(addr)
			default: dout = 0;\n""")

				address = 0
				tags = dict()

				parsedSource = []

				for text in source:
					parsedSource.append(parseLine(text))

				# find all tags
				for parsed in parsedSource:
					if(parsed['type'] == 'directive'):
						if(parsed['directive'] == '.org'):
							address = int(parsed['address'], 16)

					if('tag' in parsed.keys()):
						tags[parsed['tag']] = address

					if(parsed['type'] != 'ignore' and parsed['type'] != 'directive'):
						address += 1

				address = 0

				# assemble
				for (parsed, lineno) in zip(parsedSource, range(1, len(parsedSource) + 1)):
					if(parsed['type'] == 'directive'):
						if(parsed['directive'] == '.org'):
							address = int(parsed['address'], 16)

					elif(parsed['type'] == 'ignore'):
						pass

					else:
						rom.write("			")
						rom.write("10'h{:03x}: dout = ".format(address))
						rom.write("{")

						if(parsed['type'] == 'oponly'):
							opdefine = oponly_defines[parsed['opcode']]
							rom.write("{}, 11'b0".format(opdefine))

						elif(parsed['type'] == 'reg'):
							opdefine = reg_defines[parsed['opcode']]
							destdefine = register_defines[parsed['dest']]
							rom.write("{}, {}, 8'b0".format(opdefine, destdefine))

						elif(parsed['type'] == 'regimm'):
							opdefine = regimm_defines[parsed['opcode']]
							destdefine = register_defines[parsed['dest']]
							imm = int(parsed['src'], 16)
							rom.write("{}, {}, 8'h{:02x}".format(opdefine, destdefine, imm))

						elif(parsed['type'] == 'regoffset'):
							opdefine = regoffset_defines[parsed['opcode']]
							destdefine = register_defines[parsed['dest']]

							if('offset' in parsed.keys()):
								offset = int(parsed['offset'])

								if(abs(offset) > (2**7)-1):
									raise Exception("Offset {} too large at line {}".format(offset, lineno))

								offset = bin(0xFF & offset)[2:]
							else:
								offset = '0'

							rom.write("{}, {}, 8'b{}".format(opdefine, destdefine, offset))

						elif(parsed['type'] == 'regreg'):
							opdefine = regreg_defines[parsed['opcode']]
							destdefine = register_defines[parsed['dest']]
							rom.write("{}, {}, {}, 5'b0".format(opdefine, destdefine, register_defines[parsed['src']]))

						elif(parsed['type'] == 'regregoffset'):
							opdefine = regregoffset_defines[parsed['opcode']]
							destdefine = register_defines[parsed['dest']]
							srcdefine = register_defines[parsed['src']]

							if('offset' in parsed.keys()):
								offset = int(parsed['offset'])

								if(abs(offset) > (2**4)-1):
									raise Exception("Offset {} too large at line {}".format(offset, lineno))

								offset = bin(0x1F & offset)[2:]
							else:
								offset = '0'

							rom.write("{}, {}, {}, 5'b{}".format(opdefine, destdefine, srcdefine, offset))

						elif(parsed['type'] == 'tag'):
							if(not parsed['tagref'] in tags.keys()):
								raise Exception("Unknown tag {} at line {}".format(parsed['tagref'], lineno))

							opdefine = tag_defines[parsed['opcode']]
							tagaddr = tags[parsed['tagref']]
							rom.write("{}, 10'h{:03x}, 1'b0".format(opdefine, tagaddr))

						elif(parsed['type'] == 'addr'):
							opdefine = addr_defines[parsed['opcode']]
							addr = int(parsed['address'], 16)
							rom.write("{}, 10'h{:03x}, 1'b0".format(opdefine, addr))

						rom.write("};\n")
						address += 1

				rom.write("""		endcase
	end
endmodule\n""")




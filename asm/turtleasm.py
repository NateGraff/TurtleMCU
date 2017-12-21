#!/usr/bin/env python3

from grammar import line
from symbols import *
from pyparsing import ParseException

import sys
import ast

def parseLine(text):
	try:
		return line.parseString(text).asDict()
	except ParseException as e:
		print("Parse Error:")
		print(e.line)
		print(" " * (e.col - 1) + "^")

def main():
	if(len(sys.argv) < 2):
		print("Please supply a file to assemble.")
		exit(1)

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

			# parse the assembly
			parsedSource = [parseLine(text) for text in source]

			# find all tags
			address = 0
			tags = dict()
			for parsed in parsedSource:
				if(parsed['type'] == 'directive'):
					if(parsed['directive'] == '.org'):
						address = int(parsed['address'], 16)
					elif(parsed['directive'] == '.string'):
						tags[parsed['stringtag']] = address
						address += len(ast.literal_eval(parsed['stringcontent'])) + 1
					elif(parsed['directive'] == '.ldtag'):
						address += 2
					elif(parsed['directive'] == '.resv'):
						if('resvtag' in parsed.keys()):
							tags[parsed['resvtag']] = address
						address += int(parsed['size'])
					continue

				if('tag' in parsed.keys()):
					tags[parsed['tag']] = address

				if(parsed['type'] != 'ignore'):
					address += 1


			# assemble
			address = 0
			for (parsed, lineno) in zip(parsedSource, range(1, len(parsedSource) + 1)):
				if(parsed['type'] == 'directive'):
					if(parsed['directive'] == '.org'):
						address = int(parsed['address'], 16)

					if(parsed['directive'] == '.string'):
						for char in ast.literal_eval(parsed['stringcontent']):
							rom.write("			")
							rom.write("10'h{:03x}: dout = ".format(address))
							rom.write("{")
							rom.write("16'h{:04x}".format(ord(char)))
							rom.write("};\n")
							address += 1
						rom.write("			")
						rom.write("10'h{:03x}: dout = ".format(address))
						rom.write("{16'h0};\n")
						address += 1

					if(parsed['directive'] == '.ldtag'):
						destdefine = register_defines[parsed['dest']]

						if(not parsed['tagref'] in tags.keys()):
							raise Exception("Unknown tag {} at line {}".format(parsed['tagref'], lineno))

						tagaddr = tags[parsed['tagref']]

						rom.write("			")
						rom.write("10'h{:03x}: dout = ".format(address))
						rom.write("{")
						rom.write("`OPCODE_MVH, {}, 8'h{:02x}".format(destdefine, 0xFF & (tagaddr >> 8)))
						rom.write("};\n")
						address += 1
						rom.write("			")
						rom.write("10'h{:03x}: dout = ".format(address))
						rom.write("{")
						rom.write("`OPCODE_MVL, {}, 8'h{:02x}".format(destdefine, 0xFF & tagaddr))
						rom.write("};\n")
						address += 1

					if(parsed['directive'] == '.resv'):
						address += int(parsed['size'])

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
						srcdefine = register_defines[parsed['src']]
						rom.write("{}, {}, {}, 5'b0".format(opdefine, destdefine, srcdefine))

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

if __name__ == "__main__":
	main()



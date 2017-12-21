
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
string = CaselessKeyword('.string') + White() + Word(alphanums).setResultsName('stringtag') + White() + quotedString.addParseAction(removeQuotes).setResultsName('stringcontent')
ldtag = CaselessKeyword('.ldtag') + White() + register.setResultsName('dest') + Optional(White()) + ',' + Optional(White()) + tagref.setResultsName('tagref')
resv = CaselessKeyword('.resv') + White() + Optional(Word(alphanums)).setResultsName('resvtag') + White() + Word(nums).setResultsName('size')

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
directiveline = (Or([org,string,ldtag,resv]).setResultsName('directive') + Optional(White()) + LineEnd()).setParseAction(setTypeDirective)
line = Or([blankline,commentline,untaggedline,taggedline,directiveline])

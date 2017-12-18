
`define OPCODE_NOP  8'b00000
`define OPCODE_MV   8'b00001
`define OPCODE_MVH  8'b00010
`define OPCODE_MVL  8'b00011
`define OPCODE_LD   8'b00100
`define OPCODE_ST   8'b00101
`define OPCODE_IN   8'b00110
`define OPCODE_OUT  8'b00111
`define OPCODE_CMP  8'b01000
`define OPCODE_JMP  8'b01001
`define OPCODE_JC   8'b01010
`define OPCODE_JNC  8'b01011
`define OPCODE_JZ   8'b01100
`define OPCODE_JNZ  8'b01101
`define OPCODE_CALL 8'b01110
`define OPCODE_RET  8'b01111
`define OPCODE_PUSH 8'b10000
`define OPCODE_POP  8'b10001
`define OPCODE_ADD  8'b10010
`define OPCODE_SUB  8'b10011
`define OPCODE_AND  8'b10100
`define OPCODE_OR   8'b10101
`define OPCODE_NOT  8'b10110
`define OPCODE_XOR  8'b10111
`define OPCODE_SHR  8'b11000
`define OPCODE_SHL  8'b11001
`define OPCODE_CLR  8'b11010
`define OPCODE_LSPA 8'b11011
`define OPCODE_LSPR 8'b11100
`define OPCODE_RSP  8'b11101
`define OPCODE_LDI  8'b11110
`define OPCODE_STI  8'b11111

`define PC_DIN_OP  1'b0
`define PC_DIN_RAM 1'b1

`define RAM_ADDR_PC  2'b00
`define RAM_ADDR_SP  2'b01
`define RAM_ADDR_RF  2'b10
`define RAM_ADDR_ROM 2'b11

`define RAM_DIN_RF  2'b00
`define RAM_DIN_PC  2'b01
`define RAM_DIN_ROM 2'b10
`define RAM_DIN_SP  2'b11

`define RF_DIN_B    3'b000
`define RF_DIN_ALU  3'b001
`define RF_DIN_LOW  3'b010
`define RF_DIN_HIGH 3'b011
`define RF_DIN_RAM  3'b100
`define RF_DIN_IN   3'b101
`define RF_DIN_SP   3'b110
`define RF_DIN_ZERO 3'b111

`define SP_DIN_OP  1'b0
`define SP_DIN_RF 1'b1

`define ALU_ADD 3'b000
`define ALU_SUB 3'b001
`define ALU_AND 3'b010
`define ALU_OR  3'b011
`define ALU_NOT 3'b100
`define ALU_XOR 3'b101
`define ALU_SHL 3'b110
`define ALU_SHR 3'b111

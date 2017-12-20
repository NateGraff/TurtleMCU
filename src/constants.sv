
`define OPCODE_NOP  5'b00000
`define OPCODE_MV   5'b00001
`define OPCODE_MVH  5'b00010
`define OPCODE_MVL  5'b00011
`define OPCODE_LD   5'b00100
`define OPCODE_ST   5'b00101
`define OPCODE_IN   5'b00110
`define OPCODE_OUT  5'b00111
`define OPCODE_CMP  5'b01000
`define OPCODE_JMP  5'b01001
`define OPCODE_JC   5'b01010
`define OPCODE_JNC  5'b01011
`define OPCODE_JZ   5'b01100
`define OPCODE_JNZ  5'b01101
`define OPCODE_CALL 5'b01110
`define OPCODE_RET  5'b01111
`define OPCODE_PUSH 5'b10000
`define OPCODE_POP  5'b10001
`define OPCODE_ADD  5'b10010
`define OPCODE_SUB  5'b10011
`define OPCODE_AND  5'b10100
`define OPCODE_OR   5'b10101
`define OPCODE_NOT  5'b10110
`define OPCODE_XOR  5'b10111
`define OPCODE_SHR  5'b11000
`define OPCODE_SHL  5'b11001
`define OPCODE_CLR  5'b11010
`define OPCODE_LSPA 5'b11011
`define OPCODE_LSPR 5'b11100
`define OPCODE_RSP  5'b11101
`define OPCODE_LDI  5'b11110
`define OPCODE_STI  5'b11111

`define PC_DIN_OP  1'b0
`define PC_DIN_RAM 1'b1

`define RAM_ADDR_PC  3'b000
`define RAM_ADDR_SP  3'b001
`define RAM_ADDR_SP1 3'b010
`define RAM_ADDR_SPO 3'b011
`define RAM_ADDR_RF  3'b100
`define RAM_ADDR_ROM 3'b101

`define RAM_DIN_RF  2'b00
`define RAM_DIN_PC  2'b01
`define RAM_DIN_ROM 2'b10
`define RAM_DIN_SP  2'b11

`define RF_WRITE_OFF  2'b00
`define RF_WRITE_FULL 2'b01
`define RF_WRITE_HIGH 2'b10
`define RF_WRITE_LOW  2'b11

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

`define R0 3'b000
`define R1 3'b001
`define R2 3'b010
`define R3 3'b011
`define R4 3'b100
`define R5 3'b101
`define R6 3'b110
`define R7 3'b111

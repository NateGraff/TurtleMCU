
`include "constants.sv"

`define R0 3'b000
`define R1 3'b001
`define R2 3'b010
`define R3 3'b011
`define R4 3'b100
`define R5 3'b101
`define R6 3'b110
`define R7 3'b111

module rom(
	input wire [9:0] addr,
	output wire [15:0] dout);

	always_comb begin
		case(addr)
			10'h0000: dout = {`OPCODE_MVL, `R1, 8'h37};
			10'h0001: dout = {`OPCODE_MVL, `R2, 8'h01};
			10'h0002: dout = {`OPCODE_MVL, `R3, 8'h55};
			10'h0003: dout = {`OPCODE_STI, `R1, `R3, 5'h1F};
			10'h0004: dout = {`OPCODE_ADD, `R1, `R2, 5'b0};
			10'h0005: dout = {`OPCODE_STI, `R1, `R3, 5'h01};
			10'h0006: dout = {`OPCODE_LDI, `R4, `R3, 5'h1F};
			10'h0007: dout = {`OPCODE_OUT, `R4, 8'b0};
			10'h0008: dout = {`OPCODE_LDI, `R4, `R3, 5'h01};
			10'h0009: dout = {`OPCODE_OUT, `R4, 8'b0};
			10'h000a: dout = {`OPCODE_JMP, 10'h000a, 1'b0};
			default: dout = 0;
		endcase
	end

endmodule

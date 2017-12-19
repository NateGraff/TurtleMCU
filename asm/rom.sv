
`include "constants.sv"
module rom(
	input wire [9:0] addr,
	output wire [15:0] dout);

	always_comb begin
		case(addr)
			default: dout = 0;
			10'h000: dout = {`OPCODE_MVL, `R1, 8'h12};
			10'h001: dout = {`OPCODE_MVL, `R2, 8'h01};
			10'h002: dout = {`OPCODE_CALL, 10'h100, 1'b0};
			10'h003: dout = {`OPCODE_OUT, `R1, 8'b0};
			10'h004: dout = {`OPCODE_JMP, 10'h004, 1'b0};
			10'h100: dout = {`OPCODE_ADD, `R1, `R2, 5'b0};
			10'h101: dout = {`OPCODE_RET, 11'b0};
		endcase
	end
endmodule

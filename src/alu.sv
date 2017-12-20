
`include "constants.sv"

module alu(
	input  wire [2:0]  op,
	input  wire [15:0] arg_a,
	input  wire [15:0] arg_b,
	output reg  [15:0] out,
	output reg         carry,
	output reg         zero);

	reg [16:0] add_out;

	always_comb begin
		carry = 0;

		case(op)
			`ALU_ADD: begin
				add_out = arg_a + arg_b;
				out = add_out[15:0];
				carry = (add_out >> 16 == 17'h00001);
			end
			`ALU_SUB: begin
				out = arg_a - arg_b;

				if(~arg_a[15] & arg_b[15])
					carry = 0;
				else if(arg_a[15] & ~arg_b[15])
					carry = 1;
				else
					carry = arg_b > arg_a;
			end
			`ALU_AND: begin
				out = arg_a & arg_b;
			end
			`ALU_OR: begin
				out = arg_a | arg_b;
			end
			`ALU_NOT: begin
				out = ~arg_a;
			end
			`ALU_XOR: begin
				out = arg_a ^ arg_b;
			end
			`ALU_SHL: begin
				out = (arg_a << arg_b);
			end
			`ALU_SHR: begin
				out = (arg_a >> arg_b);
			end
		endcase

		zero = (out == 0);
	end

endmodule

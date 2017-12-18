
module alu(
	input  wire [2:0] op,
	input  wire [15:0] arg_a,
	input  wire [15:0] arg_b,
	output reg  [15:0] out,
	output reg        carry,
	output reg        zero);

	typedef enum reg [2:0] {
		ADD, SUB, AND, OR, NOT, XOR, ROL, ROR
	} op_decode_t;

	op_decode_t op_decode;

	always_comb begin
		case(op)
			3'b000: op_decode = ADD;
			3'b001: op_decode = SUB;
			3'b010: op_decode = AND;
			3'b011: op_decode =  OR;
			3'b100: op_decode = NOT;
			3'b101: op_decode = XOR;
			3'b110: op_decode = ROL;
			3'b111: op_decode = ROR;
		endcase
	end

	reg [16:0] add_out;

	always_comb begin
		case(op_decode)
			ADD: begin
				add_out = arg_a + arg_b;
				out = add_out[15:0];
				carry = (add_out >> 16 == 17'h00001);
				zero = (out == 0);
			end
			SUB: begin
				out = arg_a - arg_b;
				carry = (arg_b > arg_a);
				zero = (out == 0);
			end
			AND: begin
				out = arg_a & arg_b;
				carry = 0;
				zero = (out == 0);
			end
			OR: begin
				out = arg_a | arg_b;
				carry = 0;
				zero = (out == 0);
			end
			NOT: begin
				out = ~arg_a;
				carry = 0;
				zero = (out == 0);
			end
			XOR: begin
				out = arg_a ^ arg_b;
				carry = 0;
				zero = (out == 0);
			end
			ROL: begin
				out = (arg_a << 1) | (arg_a >> 15);
				carry = (arg_a >> 15 == 16'h0001);
				zero = (out == 0);
			end
			ROR: begin
				out = (arg_a >> 1) | (arg_a << 15);
				carry = (arg_a << 15 == 16'h8000);
				zero = (out == 0);
			end
		endcase
	end

endmodule

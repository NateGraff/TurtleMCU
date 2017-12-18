
module flags(
	input wire clk,
	input wire rst_n,
	input wire load,
	input wire carry_in,
	input wire zero_in,
	output reg carry_out,
	output reg zero_out);

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			carry_out <= 0;
			zero_out <= 0;
		end else begin
			if(load) begin
				carry_out <= carry_in;
				zero_out <= zero_in;
			end
		end
	end

endmodule

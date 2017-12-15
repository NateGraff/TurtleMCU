
module block(clk, in1, in2, out);
	input clk;
	input in1;
	input in2;
	output out;

	always @(posedge clk) begin
		out <= in1 & in2;
	end
endmodule

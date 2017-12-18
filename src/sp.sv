
module sp(
	input  wire       clk,
	input  wire       rst_n,
	input  wire       inc,
	input  wire       dec,
	output reg  [9:0] dout);

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			dout <= 0;
		end else begin
			if(inc) begin
				dout <= dout + 1;
			end else if(dec) begin
				dout <= dout - 1;
			end
		end
	end

endmodule

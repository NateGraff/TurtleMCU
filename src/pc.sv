
module pc(
	input  wire       clk,
	input  wire       rst_n,
	input  wire [9:0] din,
	input  wire       load,
	input  wire       inc,
	output reg  [9:0] dout);

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			dout <= 0;
		end else begin
			if(load) begin
				dout <= din;
			end else if(inc) begin
				dout <= dout + 1;
			end
		end
	end

endmodule

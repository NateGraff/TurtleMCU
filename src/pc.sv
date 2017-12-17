
module pc(
	input  wire       clk,
	input  wire       rst_n,
	input  wire [5:0] din,
	input  wire       load,
	output reg  [5:0] dout);

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			dout <= 0;
		end else begin
			if(load) begin
				dout <= din;
			end else begin
				dout <= dout + 1;
			end
		end
	end

endmodule
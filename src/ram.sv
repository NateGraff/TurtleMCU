
module ram(
	input  wire       clk,
	input  wire       rst_n,
	input  wire [5:0] addr,
	input  wire [7:0] din,
	input  wire       write,
	output reg  [7:0] dout);

	reg [64:0][7:0] memory;

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			memory <= 0;
		end else begin
			if(write) begin
				memory[addr] <= din;
			end else begin
				dout <= memory[addr];
			end
		end
	end

endmodule
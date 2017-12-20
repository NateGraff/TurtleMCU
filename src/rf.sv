
`include "constants.sv"

module rf(
	input  wire        clk,
	input  wire        rst_n,
	input  wire [2:0]  addr_a,
	input  wire [2:0]  addr_b,
	input  wire [15:0] din,
	input  wire [1:0]  write,
	output reg  [15:0] dout_a,
	output reg  [15:0] dout_b);

	reg [7:0][15:0] registers;

	always_comb begin
		dout_a = registers[addr_a];
		dout_b = registers[addr_b];
	end

	always @(posedge clk) begin
		if(~rst_n) begin
			registers <= 0;
		end else begin
			case(write)
				`RF_WRITE_FULL: registers[addr_a] <= din;
				`RF_WRITE_HIGH: registers[addr_a][15:8] <= din[15:8];
				`RF_WRITE_LOW:  registers[addr_a][7:0] <= din[7:0];
				default: begin end
			endcase
		end
	end

endmodule

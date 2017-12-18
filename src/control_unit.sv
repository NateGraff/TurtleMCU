
`define OPCODE_NOP  8'b00000
`define OPCODE_MV   8'b00001
`define OPCODE_MVH  8'b00010
`define OPCODE_MVL  8'b00011
`define OPCODE_LD   8'b00100
`define OPCODE_ST   8'b00101
`define OPCODE_IN   8'b00110
`define OPCODE_OUT  8'b00111
`define OPCODE_CMP  8'b01000
`define OPCODE_JMP  8'b01001
`define OPCODE_JC   8'b01010
`define OPCODE_JNC  8'b01011
`define OPCODE_JZ   8'b01100
`define OPCODE_JNZ  8'b01101
`define OPCODE_CALL 8'b01110
`define OPCODE_RET  8'b01111
`define OPCODE_PUSH 8'b10000
`define OPCODE_POP  8'b10001
`define OPCODE_ADD  8'b10010
`define OPCODE_SUB  8'b10011
`define OPCODE_AND  8'b10100
`define OPCODE_OR   8'b10101
`define OPCODE_NOT  8'b10110
`define OPCODE_XOR  8'b10111
`define OPCODE_SHR  8'b11000
`define OPCODE_SHL  8'b11001
`define OPCODE_CLR  8'b11010
`define OPCODE_LSPA 8'b11011
`define OPCODE_LSPR 8'b11100
`define OPCODE_RSP  8'b11101
`define OPCODE_LDI  8'b11110
`define OPCODE_STI  8'b11111

`define RF_DIN_B    3'b000
`define RF_DIN_ALU  3'b001
`define RF_DIN_LOW  3'b010
`define RF_DIN_HIGH 3'b011
`define RF_DIN_RAM  3'b100
`define RF_DIN_IN   3'b101
`define RF_DIN_SP   3'b110
`define RF_DIN_ZERO 3'b111

module control_unit(
	input wire clk,
	input wire rst_n,

	input wire [15:0] ram_dout,
	input wire flag_c,
	input wire flag_z,

	output reg [2:0] alu_op,

	output reg flags_load,

	output reg pc_load,
	output reg pc_sel,
	output reg pc_inc,

	output reg [9:0] rom_addr,

	output reg [1:0] ram_addr_sel,
	output reg [1:0] ram_din_sel,
	output reg ram_write,

	output reg [2:0] rf_din_sel,
	output reg rf_write,

	output reg sp_load,
	output reg sp_inc,
	output reg sp_dec,
	output reg sp_sel,

	output reg output_valid);

	typedef enum reg [1:0] {
		RESET, LOAD_ROM, FETCH, EXECUTE
	} state_t;

	state_t state;

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			state <= RESET;
		end else begin
			case(state)
				RESET: begin
					state <= LOAD_ROM;
					rom_addr = 0;
				end
				LOAD_ROM: begin
					state <= FETCH;
					rom_addr = rom_addr + 1;
				end
				FETCH: begin
					state <= EXECUTE;
				end
				EXECUTE: begin
					state <= FETCH;
				end
			endcase
		end
	end

	wire [4:0] opcode;
	assign opcode = ram_dout[15:11];

	always_comb begin
		alu_op    = 0;

		flags_load = 0;

		pc_load = 0;
		pc_sel  = 0;
		pc_inc  = 0;

		ram_addr_sel = 0;
		ram_din_sel = 0;
		ram_write    = 0;

		rf_din_sel = 0;
		rf_write   = 0;

		sp_load = 0;
		sp_inc  = 0;
		sp_dec  = 0;
		sp_sel  = 0;

		output_valid = 0;

		case(state)
			RESET: begin
				
			end
			LOAD_ROM: begin
				ram_addr_sel = 3;
				ram_din_sel = 2;
				ram_write = 1;
			end
			FETCH: begin
				
			end
			EXECUTE: begin
				pc_inc = 1;

				case({3'b0, opcode})
					default : begin

					end
					`OPCODE_MV  : begin
						rf_din_sel = `RF_DIN_B;
						rf_write = 1;
					end
					`OPCODE_MVH : begin
						rf_din_sel = `RF_DIN_HIGH;
						rf_write = 1;
					end
					`OPCODE_MVL : begin
						rf_din_sel = `RF_DIN_LOW;
						rf_write = 1;
					end
					`OPCODE_LD  : begin
						ram_addr_sel = 1;
						rf_din_sel = `RF_DIN_RAM;
						rf_write = 1;
					end
					`OPCODE_ST  : begin
						ram_addr_sel = 1;
						ram_write = 1;
					end
					`OPCODE_IN  : begin
						rf_din_sel = `RF_DIN_IN;
						rf_write = 1;
					end
					`OPCODE_OUT : begin
						output_valid = 1;
					end
					`OPCODE_CMP : begin
						alu_op = 3'b001;
					end
					`OPCODE_JMP : begin
						pc_load = 1;
					end
					`OPCODE_JC  : begin
						if(flag_c) begin
							pc_load = 1;
						end;
					end
					`OPCODE_JNC : begin
						if(~flag_c) begin
							pc_load = 1;
						end
					end
					`OPCODE_JZ  : begin
						if(flag_z) begin
							pc_load = 1;
						end
					end
					`OPCODE_JNZ : begin
						if(~flag_z) begin
							pc_load = 1;
						end
					end
					`OPCODE_CALL: begin
						pc_load = 1;
						sp_dec = 1;
						ram_din_sel = 1;
						ram_write = 1;
					end
					`OPCODE_RET : begin
						pc_load = 1;
						pc_sel = 1;
						sp_inc = 1;
					end
					`OPCODE_PUSH: begin
						ram_write = 1;
						sp_dec = 1;
					end
					`OPCODE_POP : begin
						rf_write = 1;
						rf_din_sel = `RF_DIN_RAM;
						sp_inc = 1;
					end
					`OPCODE_ADD : begin
						alu_op = 3'b000;
						rf_write = 1;
					end
					`OPCODE_SUB : begin
						alu_op = 3'b001;
						rf_write = 1;
					end
					`OPCODE_AND : begin
						alu_op = 3'b010;
						rf_write = 1;
					end
					`OPCODE_OR  : begin
						alu_op = 3'b011;
						rf_write = 1;
					end
					`OPCODE_NOT : begin
						alu_op = 3'b100;
						rf_write = 1;
					end
					`OPCODE_XOR : begin
						alu_op = 3'b101;
						rf_write = 1;
					end
					`OPCODE_SHR : begin
						alu_op = 3'b110;
						rf_write = 1;
					end
					`OPCODE_SHL : begin
						alu_op = 3'b111;
						rf_write = 1;
					end
					`OPCODE_CLR : begin
						rf_write = 1;
						rf_din_sel = `RF_DIN_ZERO;
					end
					`OPCODE_LSPA: begin
						sp_load = 1;
					end
					`OPCODE_LSPR: begin
						sp_load = 1;
						sp_sel = 1;
					end
					`OPCODE_RSP : begin
						rf_write = 1;
						rf_din_sel = `RF_DIN_SP;
					end
					`OPCODE_LDI : begin
						rf_write = 1;
						ram_addr_sel = 2;
					end
					`OPCODE_STI : begin
						ram_addr_sel = 2;
						ram_write = 1;
					end
				endcase
			end
		endcase
	end

endmodule

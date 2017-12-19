
`include "constants.sv"

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

	output reg [2:0] ram_addr_sel,
	output reg [1:0] ram_din_sel,
	output reg ram_write,

	output reg [2:0] rf_din_sel,
	output reg rf_write,

	output reg sp_load,
	output reg sp_inc,
	output reg sp_dec,
	output reg sp_sel,

	output reg output_valid);

	typedef enum reg [2:0] {
		RESET, LOAD_ROM, FETCH, EXECUTE, RAM_READ
	} state_t;

	state_t state;

	wire [4:0] opcode;
	assign opcode = ram_dout[15:11];

	always_ff @(posedge clk or negedge rst_n) begin
		if(~rst_n) begin
			state <= RESET;
		end else begin
			case(state)
				default: begin
					state <= LOAD_ROM;
					rom_addr <= 0;
				end
				LOAD_ROM: begin
					if(rom_addr == 10'h3FFF) begin
						state <= FETCH;
					end
					rom_addr <= rom_addr + 1;
				end
				FETCH: begin
					state <= EXECUTE;
				end
				EXECUTE: begin
					case(opcode)
						default:     state <= FETCH;
						`OPCODE_POP: state <= RAM_READ;
						`OPCODE_LD:  state <= RAM_READ;
						`OPCODE_LDI: state <= RAM_READ;
					endcase
				end
				RAM_READ: begin
					state <= FETCH;
				end
			endcase
		end
	end

	always_comb begin
		alu_op    = 0;

		flags_load = 0;

		pc_load = 0;
		pc_sel  = `PC_DIN_OP;
		pc_inc  = 0;

		ram_addr_sel = `RAM_ADDR_PC;
		ram_din_sel = `RAM_DIN_RF;
		ram_write    = 0;

		rf_din_sel = `RF_DIN_B;
		rf_write   = 0;

		sp_load = 0;
		sp_inc  = 0;
		sp_dec  = 0;
		sp_sel  = `SP_DIN_OP;

		output_valid = 0;

		case(state)
			default: begin
				
			end
			LOAD_ROM: begin
				ram_addr_sel = `RAM_ADDR_ROM;
				ram_din_sel = `RAM_DIN_ROM;
				ram_write = 1;
			end
			FETCH: begin
				
			end
			EXECUTE: begin
				pc_inc = 1;

				case(opcode)
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
						ram_addr_sel = `RAM_ADDR_SPO;
					end
					`OPCODE_ST  : begin
						ram_addr_sel = `RAM_ADDR_SPO;
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
						alu_op = `ALU_SUB;
						flags_load = 1;
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
						ram_din_sel = `RAM_DIN_SP;
						ram_write = 1;
					end
					`OPCODE_RET : begin
						pc_load = 1;
						pc_sel = `PC_DIN_RAM;
						sp_inc = 1;
					end
					`OPCODE_PUSH: begin
						ram_addr_sel = `RAM_ADDR_SP;
						ram_write = 1;
						sp_dec = 1;
					end
					`OPCODE_POP : begin
						ram_addr_sel = `RAM_ADDR_SP1;
						sp_inc = 1;
					end
					`OPCODE_ADD : begin
						alu_op = `ALU_ADD;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_SUB : begin
						alu_op = `ALU_SUB;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_AND : begin
						alu_op = `ALU_AND;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_OR  : begin
						alu_op = `ALU_OR;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_NOT : begin
						alu_op = `ALU_NOT;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_XOR : begin
						alu_op = `ALU_XOR;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_SHR : begin
						alu_op = `ALU_SHR;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_SHL : begin
						alu_op = `ALU_SHL;
						rf_write = 1;
						rf_din_sel = `RF_DIN_ALU;
						flags_load = 1;
					end
					`OPCODE_CLR : begin
						rf_write = 1;
						rf_din_sel = `RF_DIN_ZERO;
					end
					`OPCODE_LSPA: begin
						sp_load = 1;
						sp_sel = `SP_DIN_OP;
					end
					`OPCODE_LSPR: begin
						sp_load = 1;
						sp_sel = `SP_DIN_RF;
					end
					`OPCODE_RSP : begin
						rf_write = 1;
						rf_din_sel = `RF_DIN_SP;
					end
					`OPCODE_LDI : begin
						ram_addr_sel = `RAM_ADDR_RF;
					end
					`OPCODE_STI : begin
						ram_addr_sel = `RAM_ADDR_RF;
						ram_write = 1;
					end
				endcase
			end
			RAM_READ: begin
				rf_write = 1;
				rf_din_sel = `RF_DIN_RAM;
			end
		endcase
	end

endmodule

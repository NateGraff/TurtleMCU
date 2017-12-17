#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Vram.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

const char* TRACE_FILE = "obj_dir/ram_trace.vcd";
const vluint64_t sim_time = 100;
const uint8_t ram_size = 64;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	Vram * ram = new Vram;

	// Init trace
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	ram->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	// Initialize inputs
	uint8_t clk = 0;
	ram->clk = clk;

	ram->rst_n = 0;

	ram->addr = 0;
	ram->din = 0;
	ram->write = 0;

	// Reset
	while(!Verilated::gotFinish() && main_time < 5) {
		clk = !clk;
		ram->clk = clk;

		// Evaluate and dump trace
		ram->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	ram->rst_n = 1;

	uint8_t write_count = 0;

	while(!Verilated::gotFinish() && write_count < ram_size) {
		// Clear all inputs
		ram->addr = 0;
		ram->din = 0;
		ram->write = 0;

		// Advance the clock
		clk = !clk;
		ram->clk = clk;

		if(clk == 1) {
			// Write to the register file
			ram->addr = write_count;
			ram->din = write_count + 1;
			ram->write = 1;
			write_count++;
		}

		// Evaluate and dump trace
		ram->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	uint8_t read_count = 0;

	while(!Verilated::gotFinish() && read_count < ram_size) {
		// Clear all inputs
		ram->addr = 0;
		ram->din = 0;
		ram->write = 0;

		// Advance the clock
		clk = !clk;
		ram->clk = clk;

		if(clk == 1) {
			ram->addr = read_count;
			ram->eval();
			assert(ram->dout == (uint8_t) (read_count + 1));
			read_count++;
		}

		// Evaluate and dump trace
		ram->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	printf("RAM ALL PASS\n");

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	ram->final();
	delete ram;
	return 0;
}

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "obj_dir/Vblock.h"

#include <cstdio>

const char* TRACE_FILE = "obj_dir/trace.vcd";

Vblock *block;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	block = new Vblock;

	// Init trace
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	block->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	// Initialize inputs
	uint8_t clk = 0;
	block->clk = clk;
	block->in1 = 0;
	block->in2 = 0;

	while(!Verilated::gotFinish()) {
		// Generate clock
		if(main_time % 5 == 0) {
			clk = !clk;
			block->clk = clk;
		}

		// Input transitions
		if(main_time == 25) {
			block->in1 = 1;
		}
		if(main_time == 50) {
			block->in2 = 1;
		}
		if(main_time == 75) {
			block->in1 = 0;
		}

		// Evaluate and dump trace
		block->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;

		// End simulation
		if(main_time == 100) {
			break;
		}
	}

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	block->final();
	delete block;
	return 0;
}

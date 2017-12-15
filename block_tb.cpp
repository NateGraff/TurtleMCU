#include <verilated.h>
#include "obj_dir/Vblock.h"

#include <cstdio>

Vblock *block;

uint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	block = new Vblock;

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

		// Evaluate and print output
		block->eval();
		printf("Time: %2llu clk: %d in1: %d in2: %d out: %d\n", main_time, block->clk, block->in1, block->in2, block->out);
		
		// Advance time
		main_time++;

		// End simulation
		if(main_time == 100) {
			break;
		}
	}

	block->final();
	delete block;
	return 0;
}

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Vpc.h"

#include <cstdio>
#include <cstdlib>

const char* TRACE_FILE = "obj_dir/pc_trace.vcd";
const vluint64_t sim_time = 100;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	Vpc * pc = new Vpc;

	// Init trace
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	pc->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	// Initialize inputs
	uint8_t clk = 0;
	pc->clk = clk;

	pc->rst_n = 0;

	// Reset
	while(!Verilated::gotFinish() && main_time < 5) {
		// Advance the clock
		clk = !clk;
		pc->clk = clk;

		// Evaluate and dump trace
		pc->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	pc->rst_n = 1;


	while(!Verilated::gotFinish() && main_time < sim_time) {
		// Advance the clock
		clk = !clk;
		pc->clk = clk;

		if(clk == 1) {

		}

		// Evaluate and dump trace
		pc->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	//printf("PC ALL PASS\n");

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	pc->final();
	delete pc;
	return 0;
}

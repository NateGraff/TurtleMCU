#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Vcpu.h"

#include <cstdio>
#include <cstdlib>

const char* TRACE_FILE = "obj_dir/cpu_trace.vcd";
const vluint64_t sim_time = 4000;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	Vcpu * cpu = new Vcpu;

	// Init trace
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	cpu->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	// Initialize inputs
	uint8_t clk = 0;
	cpu->clk = clk;

	cpu->rst_n = 0;
	cpu->in_port = 0;

	// Reset
	while(!Verilated::gotFinish() && main_time < 5) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	cpu->rst_n = 1;

	while(!Verilated::gotFinish() && main_time < sim_time) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();
		tfp->dump(main_time);

		if(clk == 1) {
			if(cpu->output_valid == 1) {
				printf("time: %lu ns output: 0x%4X\n", main_time, cpu->out_port);
			}
		}
		
		// Advance time
		main_time++;
	}

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	cpu->final();
	delete cpu;
	return 0;
}

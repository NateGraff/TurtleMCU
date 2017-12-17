#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Vrf.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

const char* TRACE_FILE = "obj_dir/rf_trace.vcd";

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	Vrf * rf = new Vrf;

	// Init trace
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	rf->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	// Initialize inputs
	uint8_t clk = 0;
	rf->clk = clk;

	rf->rst_n = 0;

	rf->addr_a = 0;
	rf->addr_b = 0;
	rf->din = 0;
	rf->write = 0;

	// Reset
	while(!Verilated::gotFinish() && main_time < 5) {
		clk = !clk;
		rf->clk = clk;

		// Evaluate and dump trace
		rf->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	rf->rst_n = 1;

	uint8_t write_count = 0;

	while(!Verilated::gotFinish() && write_count < 8) {
		// Clear all inputs
		rf->addr_a = 0;
		rf->addr_b = 0;
		rf->din = 0;
		rf->write = 0;

		// Advance the clock
		clk = !clk;
		rf->clk = clk;

		if(clk == 1) {
			// Write to the register file
			rf->addr_a = write_count;
			rf->din = write_count + 1;
			rf->write = 1;
			write_count++;
		}

		// Evaluate and dump trace
		rf->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	uint8_t read_count = 0;
	int errors = 0;

	while(!Verilated::gotFinish() && read_count < 8) {
		// Clear all inputs
		rf->addr_a = 0;
		rf->addr_b = 0;
		rf->din = 0;
		rf->write = 0;

		// Advance the clock
		clk = !clk;
		rf->clk = clk;

		if(clk == 1) {
			rf->addr_a = read_count;
			rf->addr_b = 7 - read_count;
			rf->eval();

			if(rf->dout_a != (uint8_t) (read_count + 1)) {
				errors++;
				printf("A:%d Expected %d, got %d\n", rf->addr_a, (uint8_t) read_count + 1, rf->dout_a);
			}
			if(rf->dout_b != (uint8_t) (8 - read_count)) {
				errors++;
				printf("B:%d Expected %d, got %d\n", rf->addr_b, (uint8_t) (8 - read_count), rf->dout_b);
			}

			read_count++;
		}

		// Evaluate and dump trace
		rf->eval();
		tfp->dump(main_time);
		
		// Advance time
		main_time++;
	}

	if(errors == 0) {
		printf("RF ALL PASS\n");
	} else {
		printf("RF FAIL %d ERRORS\n", errors);
	}

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	rf->final();
	delete rf;
	return 0;
}

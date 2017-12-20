#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Vcpu.h"

#include <cstdio>
#include <cstdlib>

#include <queue>
using namespace std;

const char* TRACE_FILE = "obj_dir/cpu_trace.vcd";
const vluint64_t sim_time = 30000;

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
	cpu->input_valid = 0;

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

	enum {
		PORT_IDLE,
		PORT_IN,
		OUT_STRING,
		OUT_NUM
	} port_state;
	queue<uint16_t> out_queue;

	while(!Verilated::gotFinish() && main_time < sim_time) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();
		tfp->dump(main_time);

		if(clk == 1) {

			if(cpu->input_ready) {
				if(port_state == PORT_IDLE) {
					cpu->in_port = getchar();
					port_state = PORT_IN;
				}
				cpu->input_valid = 1;
			} else if(port_state == PORT_IN){
				cpu->input_valid = 0;
				port_state = PORT_IDLE;
			}

			if(cpu->output_valid) {
				if(port_state == PORT_IDLE) {
					switch(cpu->out_port) {
						case 1:
							port_state = OUT_NUM;
							break;
						case 2:
							port_state = OUT_STRING;
							break;
						default:
							port_state = PORT_IDLE;
					}
				}
				else {
					switch(port_state) {
						case OUT_NUM:
							printf("time: %lu ns output: 0x%04X\n", main_time, cpu->out_port);
							port_state = PORT_IDLE;
							break;
						case OUT_STRING:
							out_queue.push(cpu->out_port);
							break;
						default:
							port_state = PORT_IDLE;
							break;
					}

					if(!out_queue.empty()) {
						if(out_queue.back() == 0) {
							while(!out_queue.empty()) {
								printf("%c", (char) out_queue.front());
								out_queue.pop();
							}
							port_state = PORT_IDLE;
						}
					}
				}
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

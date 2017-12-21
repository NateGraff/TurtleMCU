#include <verilated.h>
#include "../obj_dir/Vcpu.h"
#include "../obj_dir/Vcpu_cpu.h"
#include "../obj_dir/Vcpu_control_unit.h"
#include "../obj_dir/Vcpu_pc.h"
#include "../obj_dir/Vcpu_rf.h"
#include "../obj_dir/Vcpu_sp.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

uint8_t getState(Vcpu * cpu) {
	return cpu->__PVT__cpu->__PVT__control_unit_i->__PVT__state;
}

uint16_t getPC(Vcpu * cpu) {
	return cpu->__PVT__cpu->__PVT__pc_i->__PVT__dout;
}

void getRegisters(Vcpu * cpu, uint16_t * registers) {
	memcpy(registers, cpu->__PVT__cpu->__PVT__rf_i->__PVT__registers, 8 * sizeof(uint16_t));
}

uint16_t getSP(Vcpu * cpu) {
	return cpu->__PVT__cpu->__PVT__sp_i->__PVT__dout;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	Vcpu * cpu = new Vcpu;

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
		
		// Advance time
		main_time++;
	}

	cpu->rst_n = 1;

	int quit = 0;
	while(!Verilated::gotFinish() && !quit) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();

		if(clk == 1 && getState(cpu) == 0b011) {
			printf("\033[2J\033[1;1H");
			printf("pc = %03x, sp = %03x\n", getPC(cpu), getSP(cpu));
			
			uint16_t registers[8];
			getRegisters(cpu, registers);
			
			for(int i = 0; i < 4; i++) {
				printf("r%d = %04x ", i, registers[i]);
			}
			printf("\n");
			for(int i = 0; i < 4; i++) {
				printf("r%d = %04x ", i+4, registers[4+i]);
			}
			printf("\n");
			printf("(n)ext, (q)uit: ");

			string command;
			cin >> command;

			switch(command[0]) {
				default:
					break;
				case 'q':
				case 'Q':
					quit = 1;
			}

			printf("\n");
		}
		
		// Advance time
		main_time++;
	}

	cpu->final();
	delete cpu;
	return 0;
}

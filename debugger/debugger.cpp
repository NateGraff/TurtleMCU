#include <verilated.h>
#include "../obj_dir/Vcpu.h"
#include "../obj_dir/Vcpu_cpu.h"
#include "../obj_dir/Vcpu_control_unit.h"
#include "../obj_dir/Vcpu_pc.h"
#include "../obj_dir/Vcpu_rf.h"
#include "../obj_dir/Vcpu_sp.h"
#include "../obj_dir/Vcpu_ram.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

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

void getRAM(Vcpu * cpu, uint16_t * ram) {
	memcpy(ram, cpu->__PVT__cpu->__PVT__ram_i->__PVT__memory, 1024 * sizeof(uint16_t));
}

uint16_t getSP(Vcpu * cpu) {
	return cpu->__PVT__cpu->__PVT__sp_i->__PVT__dout;
}

void renderScreen(Vcpu * cpu, uint16_t ram_window_start, uint16_t ram_window_end) {
	// clear terminal
	printf("\033[2J\033[1;1H");

	// print registers
	printf("---------------------------------------\n");
	printf("Registers\n");
	printf("---------------------------------------\n");
	printf(KRED "pc" KNRM " = %03x, " KYEL "sp" KNRM " = %03x\n", getPC(cpu), getSP(cpu));
	
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

	// print ram
	printf("---------------------------------------\n");
	printf("RAM\n");
	printf("---------------------------------------\n");
	uint16_t ram[1024];
	getRAM(cpu, ram);

	for(uint16_t ram_index = ram_window_start; (ram_index < ram_window_end && (ram_index + 7) < 1024); ram_index += 8) {
		printf("%03x: ", ram_index);
		for(int i = 0; i < 8; i++) {
			if(getPC(cpu) == ram_index + i) {
				printf(KRED "%04x" KNRM, ram[ram_index + i]);
			} else if(getSP(cpu) == ram_index + i) {
				printf(KYEL "%04x" KNRM, ram[ram_index + i]);
			} else {
				printf("%04x", ram[ram_index + i]);
			}
		}
		printf("\n");
	}

	printf("---------------------------------------\n");
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
	int next = 0;
	uint16_t ram_window_start = 0x000;
	uint16_t ram_window_end = 0x80;

	while(!Verilated::gotFinish() && !quit) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();

		if(clk == 1 && getState(cpu) == 0b011) {
			next = 0;
			while(!next && !quit) {
				renderScreen(cpu, ram_window_start, ram_window_end);

				printf("(n)ext, (q)uit, move to ram offset: ");

				string command;
				cin >> command;

				switch(command[0]) {
					default:
						break;
					case 'q':
					case 'Q':
						quit = 1;
						break;
					case '0':
						ram_window_start = strtol(command.c_str(), NULL, 16);
						ram_window_end = ram_window_start + 0x80;
						break;
					case 'n':
						next = 1;
						break;
				}

				printf("\n");
			}
		}
		
		// Advance time
		main_time++;
	}

	cpu->final();
	delete cpu;
	return 0;
}

#include <verilated.h>
#include "../obj_dir/Vcpu.h"
#include "../obj_dir/Vcpu_cpu.h"
#include "../obj_dir/Vcpu_control_unit.h"
#include "../obj_dir/Vcpu_pc.h"
#include "../obj_dir/Vcpu_rf.h"
#include "../obj_dir/Vcpu_sp.h"
#include "../obj_dir/Vcpu_ram.h"

#include <ncurses.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#define KRED 1
#define KGRN 2
#define KYEL 3
#define KBLU 4
#define KMAG 5
#define KCYN 6
#define KWHT 7

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

void initColors() {
	start_color();
	init_pair(KRED, COLOR_RED, COLOR_BLACK);
	init_pair(KGRN, COLOR_GREEN, COLOR_BLACK);
	init_pair(KYEL, COLOR_YELLOW, COLOR_BLACK);
	init_pair(KBLU, COLOR_BLUE, COLOR_BLACK);
	init_pair(KMAG, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(KCYN, COLOR_CYAN, COLOR_BLACK);
	init_pair(KWHT, COLOR_BLACK, COLOR_WHITE);
}

void renderScreen(Vcpu * cpu, uint16_t ram_window_start, uint16_t ram_window_end) {
	// clear terminal
	clear();
	move(0,0);

	// print registers
	printw("---------------------------------------\n");
	printw("Registers\n");
	printw("---------------------------------------\n");
	attron(COLOR_PAIR(KRED));
	printw("pc");
	attroff(COLOR_PAIR(KRED));
	printw(" = %03x  ", getPC(cpu));
	attron(COLOR_PAIR(KYEL));
	printw("sp");
	attroff(COLOR_PAIR(KYEL));
	printw(" = %03x\n", getSP(cpu));
	
	uint16_t registers[8];
	getRegisters(cpu, registers);
	
	for(int i = 0; i < 4; i++) {
		printw("r%d = %04x ", i, registers[i]);
	}
	printw("\n");
	for(int i = 0; i < 4; i++) {
		printw("r%d = %04x ", i+4, registers[4+i]);
	}
	printw("\n");

	// print ram
	printw("---------------------------------------\n");
	printw("RAM\n");
	printw("---------------------------------------\n");
	uint16_t ram[1024];
	getRAM(cpu, ram);

	for(uint16_t ram_index = ram_window_start; (ram_index < ram_window_end && (ram_index + 7) < 1024); ram_index += 8) {
		printw("%03x: ", ram_index);
		for(int i = 0; i < 8; i++) {
			if(getPC(cpu) == ram_index + i) {
				attron(COLOR_PAIR(KRED));
				printw("%04x", ram[ram_index + i]);
				attroff(COLOR_PAIR(KRED));
			} else if(getSP(cpu) == ram_index + i) {
				attron(COLOR_PAIR(KYEL));
				printw("%04x", ram[ram_index + i]);
				attroff(COLOR_PAIR(KYEL));
			} else {
				printw("%04x", ram[ram_index + i]);
			}
		}
		printw("\n");
	}

	printw("---------------------------------------\n");
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	initscr();
	initColors();

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

				printw("(n)ext, (q)uit, (r)am: ");

				char address[100];

				switch(getch()) {
					default:
						break;
					case 'q':
					case 'Q':
						printw("\nAre you sure you want to quit? (y/n): ");
						refresh();
						if(getch() == 'y') {
							quit = 1;
						}
						break;
					case 'r':
						printw("\nEnter address: ");
						refresh();
						getstr(address);
						ram_window_start = strtol(address, NULL, 16);
						ram_window_end = ram_window_start + 0x80;
						break;
					case 'n':
						next = 1;
						break;
				}
			}
		}
		
		// Advance time
		main_time++;
	}

	cpu->final();
	delete cpu;

	endwin();
	return 0;
}

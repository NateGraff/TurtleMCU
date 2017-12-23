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
Vcpu * cpu;

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

void moveRelative(WINDOW * win, int dy, int dx) {
	int y, x;
	getyx(win, y, x);
	wmove(win, y + dy, x + dx);
}

void nextLine(WINDOW * win) {
	int y, x;
	moveRelative(win, 1, 0);
	getyx(win, y, x);
	wmove(win, y, 2);
}

WINDOW * createRAMWin() {
	return newwin((LINES - 2) / 2, COLS / 2, 0, 0);
}

void renderRAMWin(WINDOW * ramwin, uint16_t ram_window_start) {
	werase(ramwin);

	box(ramwin, 0, 0);

	// print window title
	wmove(ramwin, 1, 2);
	wprintw(ramwin, "RAM");
	nextLine(ramwin);
	nextLine(ramwin);
	
	// retrieve RAM contents
	uint16_t ram[1024];
	getRAM(cpu, ram);

	int availableWidth = (COLS / 2) - 10;
	int availableHeight = (LINES - 1) / 2 - 5;
	int wordsPerLine = availableWidth / 7;
	uint16_t ram_window_end = ram_window_start + wordsPerLine * availableHeight;
	if(ram_window_end > 1024) {
		ram_window_end = 1024;
	}

	for(uint16_t ram_index = ram_window_start; ram_index < ram_window_end; ram_index += wordsPerLine) {
		wprintw(ramwin, "%03x: ", ram_index);
		for(int i = 0; i < wordsPerLine && (ram_index + i) < 1024; i++) {
			if(getPC(cpu) == ram_index + i) {
				wattron(ramwin, COLOR_PAIR(KRED));
				wprintw(ramwin, "%04x ", ram[ram_index + i]);
				wattroff(ramwin, COLOR_PAIR(KRED));
			} else if(getSP(cpu) == ram_index + i) {
				wattron(ramwin, COLOR_PAIR(KYEL));
				wprintw(ramwin, "%04x ", ram[ram_index + i]);
				wattroff(ramwin, COLOR_PAIR(KYEL));
			} else {
				wprintw(ramwin, "%04x ", ram[ram_index + i]);
			}
		}
		moveRelative(ramwin, 0, 1);
		for(int i = 0; i < 2 * wordsPerLine; i++) {
			char c = ((uint8_t * ) ram)[2*ram_index + i];
			if(c >= 32 && c <= 127) {
				wprintw(ramwin, "%c", c);
			} else {
				wprintw(ramwin, ".");
			}
		}

		nextLine(ramwin);
	}

	wrefresh(ramwin);
}

WINDOW * createRegWin() {
	return newwin(8, COLS / 2, 0, COLS / 2);
}

void renderRegWin(WINDOW * regwin) {
	werase(regwin);

	box(regwin, 0, 0);

	wmove(regwin, 1, 2);
	wprintw(regwin, "Registers");
	nextLine(regwin);
	nextLine(regwin);

	wattron(regwin, COLOR_PAIR(KRED));
	wprintw(regwin, "pc");
	wattroff(regwin, COLOR_PAIR(KRED));
	wprintw(regwin, " = %03x  ", getPC(cpu));
	wattron(regwin, COLOR_PAIR(KYEL));
	wprintw(regwin, "sp");
	wattroff(regwin, COLOR_PAIR(KYEL));
	wprintw(regwin, " = %03x", getSP(cpu));
	nextLine(regwin);
	
	uint16_t registers[8];
	getRegisters(cpu, registers);
	
	for(int i = 0; i < 4; i++) {
		wprintw(regwin, "r%d = %04x ", i, registers[i]);
	}
	nextLine(regwin);
	for(int i = 0; i < 4; i++) {
		wprintw(regwin, "r%d = %04x ", i+4, registers[4+i]);
	}
	nextLine(regwin);
	wrefresh(regwin);
}

WINDOW * createAsmWin() {
	return newwin(LINES - 10, COLS / 2, 8, COLS / 2);
}

void renderAsmWin(WINDOW * asmwin) {
	werase(asmwin);
	box(asmwin, 0, 0);
	wmove(asmwin, 1, 2);
	wprintw(asmwin, "Assembly");
	wrefresh(asmwin);
}

WINDOW * createIOWin() {
	return newwin((LINES - 2) / 2, COLS / 2, (LINES - 2) / 2, 0);
}

void renderIOWin(WINDOW * iowin) {
	werase(iowin);
	box(iowin, 0, 0);
	wmove(iowin, 1, 2);
	wprintw(iowin, "Console");
	wrefresh(iowin);
}

WINDOW * createCmdWin() {
	return newwin(2, COLS, LINES - 2, 0);
}

void setCmdWin(WINDOW * cmdwin, const char * str) {
	werase(cmdwin);
	wmove(cmdwin, 0, 0);
	wprintw(cmdwin, "%s", str);
	wrefresh(cmdwin);
}

void renderScreen(Vcpu * cpu, WINDOW * ramwin, WINDOW * regwin, WINDOW * asmwin, uint16_t ram_window_start, WINDOW * iowin, WINDOW * cmdwin) {
	clear();

	// resize windows
	wresize(ramwin, (LINES - 2) / 2, COLS / 2);
	mvwin(ramwin, 0, 0);
	wresize(regwin, 8, COLS / 2);
	mvwin(regwin, 0, COLS / 2);
	wresize(asmwin, LINES - 10, COLS / 2);
	mvwin(asmwin, 8, COLS / 2);
	wresize(iowin, (LINES - 2) / 2, COLS / 2);
	mvwin(iowin, (LINES - 2) / 2, 0);
	wresize(cmdwin, 2, COLS);
	mvwin(cmdwin, LINES - 2, 0);
	// render windows
	renderRegWin(regwin);
	renderRAMWin(ramwin, ram_window_start);
	renderAsmWin(asmwin);
	renderIOWin(iowin);
	wrefresh(cmdwin);
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	initscr();
	initColors();

	WINDOW * ramwin = createRAMWin();
	WINDOW * regwin = createRegWin();
	WINDOW * asmwin = createAsmWin();
	WINDOW * iowin  = createIOWin();
	WINDOW * cmdwin = createCmdWin();

	cpu = new Vcpu;

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

	while(!Verilated::gotFinish() && !quit) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();

		if(clk == 1 && getState(cpu) == 0b011) {
			next = 0;
			while(!next && !quit) {
				renderScreen(cpu, ramwin, regwin, asmwin, ram_window_start, iowin, cmdwin);

				setCmdWin(cmdwin, "(n)ext, (q)uit, (r)am: ");

				char address[100];

				switch(wgetch(cmdwin)) {
					default:
						break;
					case 'q':
					case 'Q':
						setCmdWin(cmdwin, "Are you sure you want to quit? (y/n): ");
						if(wgetch(cmdwin) == 'y') {
							quit = 1;
						}
						break;
					case 'r':
						setCmdWin(cmdwin, "Enter address: ");
						wgetstr(cmdwin, address);
						ram_window_start = strtol(address, NULL, 16);
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

	delwin(ramwin);
	delwin(regwin);
	delwin(asmwin);
	delwin(cmdwin);

	endwin();
	return 0;
}

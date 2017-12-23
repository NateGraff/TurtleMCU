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
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <map>

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

typedef enum {
	BREAKPOINT_OFF,
	BREAKPOINT_ON,
	BREAKPOINT_TEMP
} breakpoint_t;

vector<char> iowinoutput;
vector<string> asmtext;
uint16_t asmtextoffset = 0;
map<uint16_t, uint16_t> asmaddrtoline;
map<uint16_t, uint16_t> asmlinetoaddr;
uint16_t current_line = 0;
map<uint16_t, breakpoint_t> breakpoints;

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
	wprintw(asmwin, "Disassembly");
	nextLine(asmwin);
	nextLine(asmwin);

	for(int i = asmtextoffset; ((i - asmtextoffset) < LINES - 14) && (i < asmtext.size()); i++) {
		uint16_t addr = strtol(asmtext[i].c_str(), NULL, 16);
		if(current_line == i) {
			wattron(asmwin, COLOR_PAIR(KWHT));
			wprintw(asmwin, "%s", asmtext[i].c_str());
			wattroff(asmwin, COLOR_PAIR(KWHT));
		} else if(asmtext[i][3] != ':') {
			wattron(asmwin, COLOR_PAIR(KMAG));
			wprintw(asmwin, "%s", asmtext[i].c_str());
			wattroff(asmwin, COLOR_PAIR(KMAG));
		} else if(addr == getPC(cpu)) {
			wattron(asmwin, COLOR_PAIR(KRED));
			wprintw(asmwin, "%s", asmtext[i].c_str());
			wattroff(asmwin, COLOR_PAIR(KRED));
		} else if(breakpoints[addr] == BREAKPOINT_ON) {
			wattron(asmwin, COLOR_PAIR(KGRN));
			wprintw(asmwin, "%s", asmtext[i].c_str());
			wattroff(asmwin, COLOR_PAIR(KGRN));
		} else {
			wprintw(asmwin, "%s", asmtext[i].c_str());
		}
		nextLine(asmwin);
	}

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
	nextLine(iowin);
	nextLine(iowin);

	int consoleHeight = ((LINES - 2) / 2 - 5);
	int consoleWidth = (COLS / 2 - 4);
	for(int i = 0; i < consoleHeight; i++) {
		for(int j = 0; j < consoleWidth && (i * consoleWidth + j) < iowinoutput.size(); j++) {
			char c = iowinoutput[i * consoleWidth + j];
			if(c != '\n') {
				wprintw(iowin, "%c", c);
			} else {
				nextLine(iowin);
			}
		}
		nextLine(iowin);
	}
	wrefresh(iowin);
}

void writeIOchar(WINDOW * iowin, char c) {
	int consoleSize = ((LINES - 2) / 2 - 5) * (COLS / 2 - 4);
	iowinoutput.push_back(c);
	if(iowinoutput.size() > consoleSize) {
		iowinoutput.erase(iowinoutput.begin());
	}
}

void writeIOint(WINDOW * iowin, uint16_t i) {
	int consoleSize = ((LINES - 2) / 2 - 5) * (COLS / 2 - 4);
	char buf[100];
	sprintf(buf, "time %lu: %d\n", main_time, i);
	for(int i = 0; i < 100 & buf[i] != 0; i++) {
		iowinoutput.push_back(buf[i]);
	}
	if(iowinoutput.size() > consoleSize) {
		iowinoutput.erase(iowinoutput.begin());
	}
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

	if(argc > 1) {
		ifstream asmfile(argv[1]);
		uint16_t lineno = 0;
		for(string line; getline(asmfile, line); ) {
			asmtext.push_back(line);
			if(line[3] == ':') {
				uint16_t addr = strtol(line.c_str(), NULL, 16);
				asmlinetoaddr[lineno] = addr;
				asmaddrtoline[addr] = lineno;
			}
			lineno++;
		}
		asmfile.close();
	}

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

	enum {
		PORT_IDLE,
		PORT_OUT_INT,
		PORT_OUT_STRING,
		PORT_IN
	} port_state = PORT_IDLE;

	queue<char> input_queue;

	int run = 0;

	while(!Verilated::gotFinish() && !quit) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate and dump trace
		cpu->eval();

		// Console output
		if(clk == 1 && cpu->output_valid) {
			if(port_state == PORT_IDLE) {
				switch(cpu->out_port) {
					case 1:
						port_state = PORT_OUT_INT;
						break;
					case 2:
						port_state = PORT_OUT_STRING;
						break;
				}
			} else {
				if(port_state == PORT_OUT_INT) {
					writeIOint(iowin, cpu->out_port);
				} else if(port_state == PORT_OUT_STRING) {
					writeIOchar(iowin, (char) cpu->out_port);
				}
				port_state = PORT_IDLE;
			}
			renderScreen(cpu, ramwin, regwin, asmwin, ram_window_start, iowin, cmdwin);
		}

		// Console input
		if(clk == 1) {
			if(cpu->input_ready) {
				if(port_state == PORT_IDLE) {
					if(!input_queue.empty()) {
						port_state = PORT_IN;
						cpu->in_port = input_queue.front();
					} else {
						setCmdWin(cmdwin, "Input: ");

						char input[1000];
						if(run) {
							nodelay(cmdwin, false);
						}
						wgetstr(cmdwin, input);
						if(run) {
							nodelay(cmdwin, true);
						}

						for(int i = 0; i < 1000 && input[i] != 0; i++) {
							input_queue.push(input[i]);

							// Echo to console
							writeIOchar(iowin, input[i]);
						}
						input_queue.push('\n');
						writeIOchar(iowin, '\n');
					}
					cpu->input_valid = 1;
				}
			}
			else if(port_state == PORT_IN) {
				port_state = PORT_IDLE;
				cpu->input_valid = 0;
				input_queue.pop();
			}
		}

		// Debugger flow control
		if(clk == 1 && getState(cpu) == 0b011 && !run) {
			next = 0;
			while(!next && !quit && !run) {
				renderScreen(cpu, ramwin, regwin, asmwin, ram_window_start, iowin, cmdwin);

				setCmdWin(cmdwin, "(n)ext, (o) step over, (j/k) scroll, (b) toggle breakpoint, (r)un, (a) ram, (q)uit: ");

				char address[100];
				uint16_t addr;

				switch(wgetch(cmdwin)) {
					default:
						break;
					case 'j':
						if(current_line < (asmtext.size() - 1)) {
							current_line++;
							if((current_line - asmtextoffset) > (LINES - 15)) {
								asmtextoffset++;
							}
						}
						break;
					case 'k':
						if(current_line != 0) {
							current_line--;
							if((current_line - asmtextoffset) < 0) {
								asmtextoffset--;
							}
						}
						break;
					case 'b':
						try {
							addr = asmlinetoaddr.at(current_line);
						} catch(const exception e) {
							break;
						}
						try {
							if(breakpoints.at(addr) == BREAKPOINT_ON) {
								breakpoints[addr] = BREAKPOINT_OFF;
							} else if(breakpoints.at(addr) == BREAKPOINT_OFF) {
								breakpoints[addr] = BREAKPOINT_ON;
							}
						} catch(const exception e) {
							breakpoints[addr] = BREAKPOINT_ON;
						}
						break;
					case 'q':
					case 'Q':
						setCmdWin(cmdwin, "Are you sure you want to quit? (y/n): ");
						if(wgetch(cmdwin) == 'y') {
							quit = 1;
						}
						break;
					case 'a':
						setCmdWin(cmdwin, "Enter address: ");
						wgetstr(cmdwin, address);
						ram_window_start = strtol(address, NULL, 16);
						break;
					case 'n':
						next = 1;
						break;
					case 'r':
						run = 1;
						nodelay(cmdwin, true);
						break;
					case 'o':
						breakpoints[getPC(cpu)+1] = BREAKPOINT_TEMP;
						run = 1;
						nodelay(cmdwin, true);
						break;
				}
			}
		}

		if(breakpoints[getPC(cpu)]) {
			run = 0;
			if(breakpoints[getPC(cpu)] == BREAKPOINT_TEMP) {
				breakpoints[getPC(cpu)] = BREAKPOINT_OFF;
			}
		}

		if(run) {
			renderScreen(cpu, ramwin, regwin, asmwin, ram_window_start, iowin, cmdwin);
			setCmdWin(cmdwin, "Running -- (s) to break: ");
			if(wgetch(cmdwin) == 's') {
				run = 0;
				nodelay(cmdwin, false);
				for(map<uint16_t, breakpoint_t>::iterator it = breakpoints.begin(); it != breakpoints.end(); it++) {
					if(it->second == BREAKPOINT_TEMP) {
						it->second = BREAKPOINT_OFF;
					}
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
	delwin(iowin);
	delwin(cmdwin);

	endwin();
	return 0;
}

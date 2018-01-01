#include "debugger.h"

#include "AsmWindow.h"
#include "CmdWindow.h"
#include "IOWindow.h"
#include "RamWindow.h"
#include "RegWindow.h"

#include "escapechars.h"

#include <verilated.h>
#include "../obj_dir/Vcpu.h"
#include "../obj_dir/Vcpu_cpu.h"
#include "../obj_dir/Vcpu_control_unit.h"
#include "../obj_dir/Vcpu_pc.h"
#include "../obj_dir/Vcpu_rf.h"
#include "../obj_dir/Vcpu_sp.h"
#include "../obj_dir/Vcpu_ram.h"

#include <ncurses.h>

#include <queue>
#include <iostream>
#include <cstdbool>

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

void getRAM(Vcpu * cpu, uint16_t * ram) {
	memcpy(ram, cpu->__PVT__cpu->__PVT__ram_i->__PVT__memory, 1024 * sizeof(uint16_t));
}

uint16_t getSP(Vcpu * cpu) {
	return cpu->__PVT__cpu->__PVT__sp_i->__PVT__dout;
}

void resetCPU(Vcpu * cpu, uint8_t * clk) {
	vluint64_t end_time = main_time + 5;
	cpu->rst_n = 0;
	while(!Verilated::gotFinish() && main_time < end_time) {
		// Advance the clock
		*clk = !*clk;
		cpu->clk = *clk;

		cpu->eval();
		
		// Advance time
		main_time++;
	}
	cpu->rst_n = 1;
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

void renderScreen(RamWindow * ramwin, RegWindow * regwin, AsmWindow * asmwin, IOWindow * iowin, CmdWindow * cmdwin) {
	clear();

	// Scale and place windows
	ramwin->resizeWindow((LINES - 2) / 2, COLS / 2);
	ramwin->moveWindow(0, 0);

	regwin->resizeWindow(8, COLS / 2);
	regwin->moveWindow(0, COLS / 2);

	asmwin->resizeWindow(LINES - 10, COLS / 2);
	asmwin->moveWindow(8, COLS / 2);

	iowin->resizeWindow((LINES - 2) / 2, COLS / 2);
	iowin->moveWindow((LINES - 2) / 2, 0);
	
	cmdwin->resizeWindow(2, COLS);
	cmdwin->moveWindow(LINES - 2, 0);

	// Render windows
	regwin->render();
	ramwin->render();
	asmwin->render();
	iowin->render();
	cmdwin->render();
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	// Set up ncurses
	initscr();
	initColors();

	Vcpu * cpu = new Vcpu;

	// Initialize CPU inputs
	uint8_t clk = 0;
	cpu->clk = clk;

	cpu->rst_n = 1;
	cpu->in_port = 0;
	cpu->input_valid = 0;

	// Reset CPU
	resetCPU(cpu, &clk);

	if(argc < 2) {
		cpu->final();
		delete cpu;

		endwin();

		cerr << "Please supply a disassembly file.\n";
		exit(1);
	}

	// Create ncurses windows
	RamWindow ramwin(cpu);
	RegWindow regwin(cpu);
	AsmWindow asmwin(cpu, argv[1]);
	IOWindow iowin(cpu);
	CmdWindow cmdwin;

	// Flow control variables
	bool quit = false;
	bool next = false;
	bool run = false;

	enum {
		PORT_IDLE,
		PORT_OUT_INT,
		PORT_OUT_STRING,
		PORT_IN
	} port_state = PORT_IDLE;

	queue<uint16_t> input_queue;

	while(!Verilated::gotFinish() && !quit) {
		// Advance the clock
		clk = !clk;
		cpu->clk = clk;

		// Evaluate CPU
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
					iowin.writeInt(cpu->out_port);
				} else if(port_state == PORT_OUT_STRING) {
					iowin.writeChar((char) cpu->out_port);
				}
				port_state = PORT_IDLE;
			}
			renderScreen(&ramwin, &regwin, &asmwin, &iowin, &cmdwin);
		}

		// Console input
		if(clk == 1) {
			if(cpu->input_ready) {
				if(port_state == PORT_IDLE) {
					if(!input_queue.empty()) {
						port_state = PORT_IN;
						cpu->in_port = input_queue.front();
					} else {
						cmdwin.setText("Input: ");
						cmdwin.render();

						char input[1000];

						if(run) {
							cmdwin.setBlocking(true);
							cmdwin.gets(input, 1000);
							cmdwin.setBlocking(false);
						} else {
							cmdwin.gets(input, 1000);
						}

						// Load input into input queue
						queue<uint16_t> escapedInput = escapeString(input);
						while(!escapedInput.empty()) {
							input_queue.push(escapedInput.front());
							escapedInput.pop();
						}
						input_queue.push('\n');

						// Echo to console
						iowin.writeString(input);
						iowin.writeChar('\n');
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
			next = false;
			while(!next && !quit && !run) {
				renderScreen(&ramwin, &regwin, &asmwin, &iowin, &cmdwin);

				cmdwin.setText("(n)ext, (o) step over, (b) toggle breakpoint, (r)un, (a) set ram address, (z) reset, (q)uit: ");

				char address[100];
				uint16_t addr;

				int c = cmdwin.getc();
				switch(c) {
					default:
						break;
					case 'j':
					case KEY_DOWN:
						asmwin.scrollDown();
						break;
					case 'k':
					case KEY_UP:
						asmwin.scrollUp();
						break;
					case KEY_NPAGE:
						asmwin.scrollDown(16);
						break;
					case KEY_PPAGE:
						asmwin.scrollUp(16);
						break;
					case 'b':
						asmwin.toggleBreakpoint();
						break;
					case 'q':
					case 'Q':
						cmdwin.setText("Are you sure you want to quit? (y/n): ");
						if(cmdwin.getc() == 'y') {
							quit = true;
						}
						break;
					case 'a':
						cmdwin.setText("Enter address: ");
						cmdwin.gets(address, 100);
						ramwin.window_start = strtol(address, NULL, 16);
						break;
					case 'n':
						next = true;
						break;
					case 'r':
						run = true;
						cmdwin.setBlocking(false);
						break;
					case 'o':
						asmwin.setTempBreakpoint(getPC(cpu) + 1);
						run = true;
						cmdwin.setBlocking(false);
						break;
					case 'z':
						resetCPU(cpu, &clk);
						iowin.clear();
						next = true;
						break;
				}
			}
		}

		if(asmwin.breakpoints[getPC(cpu)] != BREAKPOINT_OFF) {
			run = false;
			cmdwin.setBlocking(true);
			asmwin.clearTempBreakpoints();
		}

		if(run) {
			renderScreen(&ramwin, &regwin, &asmwin, &iowin, &cmdwin);
			cmdwin.setText("Running -- (s) to break: ");
			if(cmdwin.getc() == 's') {
				run = false;
				cmdwin.setBlocking(true);
				asmwin.clearTempBreakpoints();
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

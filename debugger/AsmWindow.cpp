#include "AsmWindow.h"
#include "debugger.h"
#include <fstream>

AsmWindow::AsmWindow(Vcpu * cpu, char * disasm_filename) {
	this->cpu = cpu;

	ifstream asmfile(disasm_filename);

	uint16_t lineno = 0;
	for(string line; getline(asmfile, line); ) {
		disasm.push_back(line);
		if(line[3] == ':') {
			uint16_t addr = strtol(line.c_str(), NULL, 16);
			linetoaddr[lineno] = addr;
			addrtoline[addr] = lineno;
			breakpoints[addr] = BREAKPOINT_OFF;
		}
		lineno++;
	}
	asmfile.close();

	current_line = 0;
	text_offset = 0;
}

void AsmWindow::render() {
	werase(win);
	box(win, 0, 0);

	wmove(win, 1, 2);
	wprintw(win, "Disassembly");
	nextLine();
	nextLine();

	for(int i = text_offset; ((i - text_offset) < height - 4) && (i < disasm.size()); i++) {
		uint16_t addr = strtol(disasm[i].c_str(), NULL, 16);
		if(current_line == i) {
			wattron(win, COLOR_PAIR(KWHT));
			wprintw(win, "%s", disasm[i].c_str());
			wattroff(win, COLOR_PAIR(KWHT));
		} else if(disasm[i][3] != ':') {
			wattron(win, COLOR_PAIR(KMAG));
			wprintw(win, "%s", disasm[i].c_str());
			wattroff(win, COLOR_PAIR(KMAG));
		} else if(addr == getPC(cpu)) {
			wattron(win, COLOR_PAIR(KRED));
			wprintw(win, "%s", disasm[i].c_str());
			wattroff(win, COLOR_PAIR(KRED));
		} else if(breakpoints[addr] == BREAKPOINT_ON) {
			wattron(win, COLOR_PAIR(KGRN));
			wprintw(win, "%s", disasm[i].c_str());
			wattroff(win, COLOR_PAIR(KGRN));
		} else {
			wprintw(win, "%s", disasm[i].c_str());
		}
		nextLine();
	}

	refresh();
}

void AsmWindow::toggleBreakpoint() {
	try {
		toggleBreakpoint(linetoaddr.at(current_line));
	} catch(const exception e) {
		// Can't set breakpoint
	}
}

void AsmWindow::toggleBreakpoint(uint16_t addr) {
	try {
		if(breakpoints.at(addr) != BREAKPOINT_OFF) {
			breakpoints[addr] = BREAKPOINT_OFF;
		} else {
			breakpoints[addr] = BREAKPOINT_ON;
		}
	} catch(const exception e) {
		// Can't set a breakpoint
	}
}

void AsmWindow::setTempBreakpoint(uint16_t addr) {
	try {
		if(breakpoints[addr] == BREAKPOINT_OFF) {
			breakpoints[addr] = BREAKPOINT_TEMP;
		}
	} catch(const exception e) {
		// Can't set a breakpoint
	}
}

void AsmWindow::clearTempBreakpoints() {
	for(map<uint16_t, breakpoint_t>::iterator it = breakpoints.begin(); it != breakpoints.end(); it++) {
		if(it->second == BREAKPOINT_TEMP) {
			it->second = BREAKPOINT_OFF;
		}
	}
}

void AsmWindow::scrollDown() {
	scrollDown(1);
}

void AsmWindow::scrollDown(int lines) {
	if(current_line + lines < (disasm.size() - 1)) {
		current_line += lines;
	} else {
		current_line = disasm.size() - 1;
	}

	if((current_line - text_offset) > (height - 5)) {
		text_offset += lines;
	}
}

void AsmWindow::scrollUp() {
	scrollUp(1);
}

void AsmWindow::scrollUp(int lines) {
	if(current_line - lines > 0) {
		current_line -= lines;
		if((current_line - text_offset) < 0) {
			text_offset -= lines;
		}
	} else {
		current_line = 0;
		text_offset = 0;
	}
}

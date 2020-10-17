#include "RegWindow.h"
#include "debugger.h"

using namespace std;

RegWindow::RegWindow(Vcpu * cpu) {
	this->cpu = cpu;
}

void RegWindow::render() {
	werase(win);
	box(win, 0, 0);

	// print window title
	wmove(win, 1, 2);
	wprintw(win, "Registers");
	nextLine();
	nextLine();
	
	wattron(win, COLOR_PAIR(KRED));
	wprintw(win, "pc");
	wattroff(win, COLOR_PAIR(KRED));
	wprintw(win, " = %03x  ", getPC(cpu));
	wattron(win, COLOR_PAIR(KYEL));
	wprintw(win, "sp");
	wattroff(win, COLOR_PAIR(KYEL));
	wprintw(win, " = %03x", getSP(cpu));
	nextLine();
	
	uint16_t registers[8];
	getRegisters(cpu, registers);
	
	for(int i = 0; i < 4; i++) {
		wprintw(win, "r%d = %04x ", i, registers[i]);
	}
	nextLine();
	for(int i = 0; i < 4; i++) {
		wprintw(win, "r%d = %04x ", i+4, registers[4+i]);
	}

	refresh();
}

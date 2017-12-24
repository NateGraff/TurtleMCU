#include "RamWindow.h"
#include "debugger.h"
#include <cstring>

RamWindow::RamWindow(Vcpu * cpu) {
	this->cpu = cpu;
	window_start = 0;
}

void RamWindow::render() {
	werase(win);
	box(win, 0, 0);

	// print window title
	wmove(win, 1, 2);
	wprintw(win, "RAM");
	nextLine();
	nextLine();
	
	// retrieve RAM contents
	uint16_t ram[1024];
	getRAM(cpu, ram);

	int availableWidth = width - 10;
	int availableHeight = height - 5;
	int wordsPerLine = availableWidth / 7;
	uint16_t window_end = window_start + wordsPerLine * availableHeight;
	if(window_end > 1024) {
		window_end = 1024;
	}

	for(uint16_t ram_index = window_start; ram_index < window_end; ram_index += wordsPerLine) {
		wprintw(win, "%03x: ", ram_index);
		for(int i = 0; i < wordsPerLine && (ram_index + i) < 1024; i++) {
			if(getPC(cpu) == ram_index + i) {
				wattron(win, COLOR_PAIR(KRED));
				wprintw(win, "%04x ", ram[ram_index + i]);
				wattroff(win, COLOR_PAIR(KRED));
			} else if(getSP(cpu) == ram_index + i) {
				wattron(win, COLOR_PAIR(KYEL));
				wprintw(win, "%04x ", ram[ram_index + i]);
				wattroff(win, COLOR_PAIR(KYEL));
			} else {
				wprintw(win, "%04x ", ram[ram_index + i]);
			}
		}

		int y, x;
		getyx(win, y, x);
		wmove(win, y, width - (2 * wordsPerLine + 2));
		for(int i = 0; i < 2 * wordsPerLine && (ram_index + i/2) < 1024; i++) {
			char c = ((uint8_t * ) ram)[2*ram_index + i];
			if(c >= 32 && c <= 127) {
				wprintw(win, "%c", c);
			} else {
				wprintw(win, ".");
			}
		}

		nextLine();
	}

	refresh();
}

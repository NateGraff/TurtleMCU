#include "IOWindow.h"

IOWindow::IOWindow(Vcpu * cpu) {
	this->cpu = cpu;
}

void IOWindow::render() {
	werase(win);
	box(win, 0, 0);
	wmove(win, 1, 2);
	wprintw(win, "Console");
	nextLine();
	nextLine();

	int consoleHeight = (height - 5);
	int consoleWidth = (width - 4);
	int y = 0;
	int x = 0;

	for(int i = 0; i < output.size(); i++) {
		char c = output[i];
		if(c == '\n') {
			nextLine();
			y++;
			x = 0;
		} else {
			wprintw(win, "%c", c);
			x++;
		}

		if(x > consoleWidth) {
			nextLine();
			x = 0;
			y++;
		}

		if(y > consoleHeight) {
			break;
		}
	}

	refresh();
}

void IOWindow::writeChar(char c) {
	output.push_back(c);
	shrink();
}

void IOWindow::writeInt(uint16_t d) {
	char buf[100];
	sprintf(buf, "%d\n", d);

	for(int i = 0; i < 100 & buf[i] != 0; i++) {
		output.push_back(buf[i]);
	}

	shrink();
}

void IOWindow::writeString(char * str) {
	for(int i = 0; str[i] != 0; i++) {
		output.push_back(str[i]);
	}

	shrink();
}

void IOWindow::shrink() {
	int consoleSize = (height - 5) * (width - 4);
	while(output.size() > consoleSize) {
		output.erase(output.begin());
	}
}

void IOWindow::clear() {
	output.erase(output.begin(), output.end());
}

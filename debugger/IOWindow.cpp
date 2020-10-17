#include "IOWindow.h"

using namespace std;

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
	int x = 0;
	int y = 0;

	// Count backwards so we find the spot to truncate the beginning
	for(int i = output.size() - 1; i >= 0; i--) {
		// Increment position
		if(output[i] == '\n') {
			x = 0;
			y++;
		} else {
			x++;
		}

		if(x > (width - 5)) {
			x = 0;
			y++;
		}

		// If we max out the height, truncate from the beginning
		if(y >= (height - 5)) {
			output.erase(output.begin(), output.begin() + i);
			break;
		}
	}
}

void IOWindow::clear() {
	output.erase(output.begin(), output.end());
}

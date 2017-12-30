#include "CmdWindow.h"

CmdWindow::CmdWindow() {
	// Enable function key input
	keypad(win, true);
}

void CmdWindow::setText(string text) {
	this->text = text;
	render();
}

void CmdWindow::render() {
	werase(win);
	wmove(win, 0, 0);
	wprintw(win, "%s", text.c_str());
	refresh();
}

int CmdWindow::getc() {
	return wgetch(win);
}

void CmdWindow::gets(char * input, size_t n) {
	wgetnstr(win, input, n);
}

void CmdWindow::setBlocking(bool blocking) {
	nodelay(win, !blocking);
}
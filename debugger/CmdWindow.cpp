#include "CmdWindow.h"

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

char CmdWindow::getc() {
	return wgetch(win);
}

void CmdWindow::gets(char * input, size_t n) {
	wgetnstr(win, input, n);
}

void CmdWindow::setBlocking(bool blocking) {
	nodelay(win, !blocking);
}
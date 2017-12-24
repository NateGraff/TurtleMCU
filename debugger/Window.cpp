#include "Window.h"

Window::Window() {
	win = newwin(0, 0, 0, 0);
}

Window::~Window() {
	delwin(this->win);
}

void Window::render() {
	werase(win);
	box(win, 0, 0);
}

void Window::resizeWindow(int height, int width) {
	this->height = height;
	this->width = width;
	wresize(win, height, width);
}

void Window::moveWindow(int starty, int startx) {
	this->startx = startx;
	this->starty = starty;
	mvwin(win, starty, startx);
}

void Window::refresh() {
	wrefresh(win);
}

void Window::moveRelative(int dy, int dx) {
	int y, x;
	getyx(win, y, x);
	wmove(win, y + dy, x + dx);
}

void Window::nextLine() {
	int y, x;
	moveRelative(1, 0);
	getyx(win, y, x);
	wmove(win, y, 2);
}

#pragma once

#include <ncurses.h>

using namespace std;

class Window {
protected:
	WINDOW * win;
	int width;
	int height;
	int startx;
	int starty;
public:
	Window();
	~Window();
	void render();
	void resizeWindow(int height, int width);
	void moveWindow(int starty, int startx);
	void refresh();
	void moveRelative(int dy, int dx);
	void nextLine();
};

#pragma once

#include "Window.h"
#include <string>
#include <cstdbool>

using namespace std;

class CmdWindow : public Window {
protected:
	string text;
public:
	CmdWindow();
	void setText(string text);
	void render();
	int getc();
	void gets(char * input, size_t n);
	void setBlocking(bool blocking);
};

#pragma once

#include "Window.h"
#include <string>
#include <cstdbool>

using namespace std;

class CmdWindow : public Window {
protected:
	string text;
public:
	void setText(string text);
	void render();
	char getc();
	void gets(char * input, size_t n);
	void setBlocking(bool blocking);
};

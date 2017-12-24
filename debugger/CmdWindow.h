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
	void gets(char * input);
	void setBlocking(bool blocking);
};

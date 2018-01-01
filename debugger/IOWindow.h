#pragma once

#include "Window.h"
#include "../obj_dir/Vcpu.h"
#include <vector>

class IOWindow : public Window {
protected:
	Vcpu * cpu;
	vector<char> output;
	void shrink();
public:
	IOWindow(Vcpu * cpu);
	void render();
	void writeChar(char c);
	void writeInt(uint16_t d);
	void writeString(char * str);
	void clear();
};

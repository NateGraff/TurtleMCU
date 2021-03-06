#pragma once

#include "Window.h"
#include "../obj_dir/Vcpu.h"

class RamWindow : public Window {
protected:
	Vcpu * cpu;
public:
	RamWindow(Vcpu * cpu);
	int window_start;
	void render();
};

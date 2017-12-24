#pragma once

#include "Window.h"
#include "../obj_dir/Vcpu.h"

class RegWindow : public Window {
protected:
	Vcpu * cpu;
public:
	RegWindow(Vcpu * cpu);
	void render();
};

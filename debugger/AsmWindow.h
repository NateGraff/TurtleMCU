#pragma once

#include "Window.h"
#include "../obj_dir/Vcpu.h"
#include <cstdint>
#include <vector>
#include <map>
#include <string>

using namespace std;

typedef map<uint16_t, uint16_t> addrmap_t;

typedef enum {
	BREAKPOINT_OFF,
	BREAKPOINT_ON,
	BREAKPOINT_TEMP
} breakpoint_t;

class AsmWindow : public Window {
protected:
	Vcpu * cpu;
	vector<string> disasm;
public:
	int current_line;
	int text_offset;
	addrmap_t linetoaddr;
	addrmap_t addrtoline;
	map<uint16_t, breakpoint_t> breakpoints;
	AsmWindow(Vcpu * cpu, char * disasm_filename);
	void render();
	void toggleBreakpoint();
	void toggleBreakpoint(uint16_t addr);
	void setTempBreakpoint(uint16_t addr);
	void clearBreakpoints();
	void clearTempBreakpoints();
	void scrollDown();
	void scrollDown(int lines);
	void scrollUp();
	void scrollUp(int lines);
};

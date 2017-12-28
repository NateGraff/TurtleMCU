
SRC_DIR = src
TEST_DIR = test
ASM_DIR = asm
DEBUGGER_DIR = debugger

DEBUG_SRC_C = $(DEBUGGER_DIR)/*.cpp
DEBUG_SRC_H = $(DEBUGGER_DIR)/*.h
DEBUG_SRC = $(DEBUG_SRC_C) $(DEBUG_SRC_H)

default: testio

#
# Debugger Interface Targets
#

# CPU Integration Test
test1: $(SRC_DIR)/*.sv $(DEBUG_SRC) $(ASM_DIR)/test.s
	$(MAKE) -j -C $(ASM_DIR) test1
	cp $(ASM_DIR)/rom.sv $(SRC_DIR)/rom.sv
	verilator -cc $(SRC_DIR)/cpu.sv -I$(SRC_DIR) -O0 --exe $(DEBUG_SRC_C) -LDFLAGS -lncurses -o debugger
	$(MAKE) -j -C obj_dir/ -f Vcpu.mk
	./obj_dir/debugger $(ASM_DIR)/rom.dis

# Standard I/O Test
testio: $(SRC_DIR)/*.sv $(DEBUG_SRC) $(ASM_DIR)/testio.s
	$(MAKE) -j -C $(ASM_DIR) testio
	cp $(ASM_DIR)/rom.sv $(SRC_DIR)/rom.sv
	verilator -cc $(SRC_DIR)/cpu.sv -I$(SRC_DIR) -O0 --exe $(DEBUG_SRC_C) -LDFLAGS -lncurses -o debugger
	$(MAKE) -j -C obj_dir/ -f Vcpu.mk
	./obj_dir/debugger $(ASM_DIR)/rom.dis

#
# Challenges Inspired by Microcorruption CTF
#
challenge1: $(SRC_DIR)/*.sv $(DEBUG_SRC) $(ASM_DIR)/challenges/challenge1.s
	$(MAKE) -j -C $(ASM_DIR) challenge1
	cp $(ASM_DIR)/rom.sv $(SRC_DIR)/rom.sv
	verilator -cc $(SRC_DIR)/cpu.sv -I$(SRC_DIR) -O0 --exe $(DEBUG_SRC_C) -LDFLAGS -lncurses -o debugger
	$(MAKE) -j -C obj_dir/ -f Vcpu.mk
	./obj_dir/debugger $(ASM_DIR)/rom.dis

#
# Tests
#
integration: cpu units
	./obj_dir/Vcpu

units: rf ram alu pc
	./obj_dir/Vrf
	./obj_dir/Vram
	./obj_dir/Valu
	./obj_dir/Vpc

cpu: $(SRC_DIR)/*.sv $(TEST_DIR)/cpu_tb.cpp $(ASM_DIR)/test.s
	$(MAKE) -j -C $(ASM_DIR) test1
	cp $(ASM_DIR)/rom.sv $(SRC_DIR)/rom.sv
	verilator -cc $(SRC_DIR)/cpu.sv -I$(SRC_DIR) --trace --trace-structs --exe $(TEST_DIR)/cpu_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vcpu.mk

rf: $(SRC_DIR)/rf.sv $(TEST_DIR)/rf_tb.cpp
	verilator -cc $(SRC_DIR)/rf.sv -I$(SRC_DIR) --trace --trace-structs --exe $(TEST_DIR)/rf_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vrf.mk

ram: $(SRC_DIR)/ram.sv $(TEST_DIR)/ram_tb.cpp
	verilator -cc $(SRC_DIR)/ram.sv -I$(SRC_DIR) --trace --trace-structs --exe $(TEST_DIR)/ram_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vram.mk

alu: $(SRC_DIR)/alu.sv $(TEST_DIR)/alu_tb.cpp
	verilator -cc $(SRC_DIR)/alu.sv -I$(SRC_DIR) --trace --trace-structs --exe $(TEST_DIR)/alu_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Valu.mk

pc: $(SRC_DIR)/pc.sv $(TEST_DIR)/pc_tb.cpp
	verilator -cc $(SRC_DIR)/pc.sv -I$(SRC_DIR) --trace --trace-structs --exe $(TEST_DIR)/pc_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vpc.mk

clean:
	rm -rf obj_dir
	rm -f src/rom.sv
	rm -f asm/rom.sv
	rm -f asm/rom.dis
	rm -rf asm/__pycache__


SRC_DIR = src
TEST_DIR = test
ASM_DIR = asm

default: cpu

rom: $(ASM_DIR)/test.s
	$(MAKE) -j -C $(ASM_DIR)
	cp $(ASM_DIR)/rom.sv $(SRC_DIR)/rom.sv

cpu: rom $(SRC_DIR)/*.sv $(TEST_DIR)/cpu_tb.cpp
	verilator -cc $(SRC_DIR)/cpu.sv -I$(SRC_DIR) --trace --exe $(TEST_DIR)/cpu_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vcpu.mk
	./obj_dir/Vcpu

units: rf ram alu pc
	./obj_dir/Vrf
	./obj_dir/Vram
	./obj_dir/Valu
	./obj_dir/Vpc

rf: $(SRC_DIR)/rf.sv $(TEST_DIR)/rf_tb.cpp
	verilator -cc $(SRC_DIR)/rf.sv --trace --exe $(TEST_DIR)/rf_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vrf.mk

ram: $(SRC_DIR)/ram.sv $(TEST_DIR)/ram_tb.cpp
	verilator -cc $(SRC_DIR)/ram.sv --trace --exe $(TEST_DIR)/ram_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vram.mk

alu: $(SRC_DIR)/alu.sv $(TEST_DIR)/alu_tb.cpp
	verilator -cc $(SRC_DIR)/alu.sv --trace --exe $(TEST_DIR)/alu_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Valu.mk

pc: $(SRC_DIR)/pc.sv $(TEST_DIR)/pc_tb.cpp
	verilator -cc $(SRC_DIR)/pc.sv --trace --exe $(TEST_DIR)/pc_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vpc.mk

clean:
	rm -r obj_dir

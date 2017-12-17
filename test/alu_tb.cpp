#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Valu.h"

#include <cstdio>

VerilatedVcdC* tfp = new VerilatedVcdC;
const char* TRACE_FILE = "obj_dir/alu_trace.vcd";

#define ADD 0
#define SUB 1
#define AND 2
#define OR  3
#define NOT 4
#define XOR 5
#define ROL 6
#define ROR 7

Valu *alu;

vluint64_t main_time = 0;

double sc_time_stamp() {
	return main_time;
}

void aluOp(uint8_t op, uint8_t arg_a, uint8_t arg_b) {
	alu->op = op;
	alu->arg_a = arg_a;
	alu->arg_b = arg_b;
	alu->eval();
	tfp->dump(main_time);

	main_time++;
}

int testOp(uint8_t op, uint8_t arg_a, uint8_t arg_b, uint8_t out, uint8_t carry=0) {
	int errors = 0;

	aluOp(op, arg_a, arg_b);
	
	if(alu->out != out) {
		errors++;
		printf("FAIL OP %d arg_a %d arg_b %d expected out = %d, got %d\n", op, arg_a, arg_b, out, alu->out);
	}
	if(alu->carry != carry) {
		errors++;
		printf("FAIL OP %d arg_a %d arg_b %d expected carry = %d, got %d\n", op, arg_a, arg_b, carry, alu->carry);
	}
	if((out == 0) && (alu->zero != 1)) {
		errors++;
		printf("FAIL OP %d arg_a %d arg_b %d expected zero = %d, got %d\n", op, arg_a, arg_b, (out == 0), alu->zero);
	}
	
	return errors;
}

int testAdd(uint8_t arg_a, uint8_t arg_b) {
	uint8_t carry = ((arg_a + arg_b) > 255) ? 1 : 0;
	return testOp(ADD, arg_a, arg_b, arg_a + arg_b, carry);
}

int testSub(uint8_t arg_a, uint8_t arg_b) {
	uint8_t carry = (arg_b > arg_a) ? 1 : 0;
	return testOp(SUB, arg_a, arg_b, arg_a - arg_b, carry);
}

int testRol(uint8_t arg_a) {
	uint8_t carry = (arg_a & 0x80) ? 1 : 0;
	return testOp(ROL, arg_a, 0, (arg_a << 1) | (arg_a >> 7), carry);
}

int testRor(uint8_t arg_a) {
	uint8_t carry = (arg_a & 0x01) ? 1 : 0;
	return testOp(ROR, arg_a, 0, (arg_a >> 1) | (arg_a << 7), carry);
}

int verifyAlu() {
	int errors = 0;

	// Initialize inputs
	alu->op = 0;
	alu->arg_a = 0;
	alu->arg_b = 0;

	// ADD
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 256; j++) {
			errors += testAdd(i, j);
		}
	}

	// SUB
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 256; j++) {
			errors += testSub(i, j);
		}
	}

	// AND
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 256; j++) {
			errors += testOp(AND, i, j, i & j);
		}
	}

	// OR
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 256; j++) {
			errors += testOp(OR, i, j, i | j);
		}
	}

	// NOT
	for(int i = 0; i < 256; i++) {
		errors += testOp(NOT, i, 0, ~i);
	}

	// XOR
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 256; j++) {
			errors += testOp(XOR, i, j, i ^ j);
		}
	}

	// ROL
	for(int i = 0; i < 256; i++) {
		errors += testRol(i);
	}

	// ROR
	for(int i = 0; i < 256; i++) {
		errors += testRor(i);
	}

	return errors;
}

int main(int argc, char ** argv) {
	Verilated::commandArgs(argc, argv);

	alu = new Valu;

	Verilated::traceEverOn(true);
	alu->trace(tfp, 99);
	tfp->open(TRACE_FILE);

	int errors = verifyAlu();

	if(errors == 0) {
		printf("ALU ALL PASS\n");
	} else {
		printf("FAIL ALU %d ERRORS\n", errors);
	}

	tfp->close();
	printf("Wrote output trace to %s\n", TRACE_FILE);

	alu->final();
	delete alu;

	return 0;
}

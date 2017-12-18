#include <verilated.h>
#include <verilated_vcd_c.h>
#include "../obj_dir/Valu.h"

#include <cstdio>
#include <cstdlib>

VerilatedVcdC* tfp = new VerilatedVcdC;
const char* TRACE_FILE = "obj_dir/alu_trace.vcd";

const int test_rounds = 50;

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

void aluOp(uint16_t op, uint16_t arg_a, uint16_t arg_b) {
	alu->op = op;
	alu->arg_a = arg_a;
	alu->arg_b = arg_b;
	alu->eval();
	tfp->dump(main_time);

	main_time++;
}

int testOp(uint16_t op, uint16_t arg_a, uint16_t arg_b, uint16_t out, uint16_t carry=0) {
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

int testAdd(uint16_t arg_a, uint16_t arg_b) {
	uint16_t carry = ((arg_a + arg_b) >> 16) ? 1 : 0;
	return testOp(ADD, arg_a, arg_b, arg_a + arg_b, carry);
}

int testSub(uint16_t arg_a, uint16_t arg_b) {
	uint16_t carry = (arg_b > arg_a) ? 1 : 0;
	return testOp(SUB, arg_a, arg_b, arg_a - arg_b, carry);
}

int testRol(uint16_t arg_a) {
	uint16_t carry = (arg_a & 0x8000) ? 1 : 0;
	return testOp(ROL, arg_a, 0, (arg_a << 1) | (arg_a >> 15), carry);
}

int testRor(uint16_t arg_a) {
	uint16_t carry = (arg_a & 0x0001) ? 1 : 0;
	return testOp(ROR, arg_a, 0, (arg_a >> 1) | (arg_a << 15), carry);
}

int verifyAlu() {
	int errors = 0;

	// Initialize inputs
	alu->op = 0;
	alu->arg_a = 0;
	alu->arg_b = 0;

	// ADD
	for(int i = 0; i < test_rounds; i++) {
		errors += testAdd(rand(), rand());
	}

	// SUB
	for(int i = 0; i < test_rounds; i++) {
		errors += testSub(rand(), rand());
	}

	// AND
	for(int i = 0; i < test_rounds; i++) {
		uint16_t a = rand();
		uint16_t b = rand();
		errors += testOp(AND, a, b, a & b);
	}

	// OR
	for(int i = 0; i < test_rounds; i++) {
		uint16_t a = rand();
		uint16_t b = rand();
		errors += testOp(OR, a, b, a | b);
	}

	// NOT
	for(int i = 0; i < test_rounds; i++) {
		uint16_t a = rand();
		errors += testOp(NOT, a, 0, ~a);
	}

	// XOR
	for(int i = 0; i < test_rounds; i++) {
		uint16_t a = rand();
		uint16_t b = rand();
		errors += testOp(XOR, a, b, a ^ b);
	}

	// ROL
	for(int i = 0; i < test_rounds; i++) {
		errors += testRol(rand());
	}

	// ROR
	for(int i = 0; i < test_rounds; i++) {
		errors += testRor(rand());
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

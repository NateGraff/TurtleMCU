#pragma once

#include "../obj_dir/Vcpu.h"
#include <cstdint>

#define KRED 1
#define KGRN 2
#define KYEL 3
#define KBLU 4
#define KMAG 5
#define KCYN 6
#define KWHT 7

uint8_t getState(Vcpu * cpu);
uint16_t getPC(Vcpu * cpu);
void getRegisters(Vcpu * cpu, uint16_t * registers);
void getRAM(Vcpu * cpu, uint16_t * ram);
uint16_t getSP(Vcpu * cpu);

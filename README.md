# TurtleMCU

A toy microprocessor written in SystemVerilog by Nathaniel Graff

## Introduction

To take a break from my Master's thesis, I started a project with three goals:

1. To better familiarize myself with SystemVerilog
2. To try out an open source Verilog simulator (Verilator) as an alternative to Xilinx Vivado
3. To play around with the idea of writing a von Neumman architecture CPU

As it stands, there are now a number of components to the project:

- A 16-bit von Neumann architecture microprocessor written in SystemVerilog, heavily inspired by the RAT Microprocessor created by Dr. Bryan Mealy for the Cal Poly SLO Computer Engineering program.
- A very simple instruction set architecture for the microprocessor which fits in a fixed 16-bit word length.
- An assembler for the TurtleMCU assembly language written in Python 3 using pyparsing
- An interactive debugger written in C++ using the Verilator simulation of the CPU.
- Some (very incomplete) verification unit tests of the individual modules of the CPU using Verilator

## Software Requirements

- make
- GCC/Clang
- Verilator
- ncurses
- Python 3
- pyparsing
- GTKWave (or other waveform viewer of .vcd files)

## Simulating the CPU

Simply run `make` in the top level directory. Values output from the CPU will be shown on STDOUT. The trace of the internal CPU signals can be found in obj_dir/cpu_trace.vcd.

The code executed by the CPU is located in asm/test.s.

## Copyright

All files are copyright Nathaniel Graff 2017 and distributed under the license found in the root directory of this repository.

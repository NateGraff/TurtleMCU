
default:
	verilator -cc block.sv --trace --exe block_tb.cpp
	$(MAKE) -j -C obj_dir/ -f Vblock.mk
	./obj_dir/Vblock

clean:
	rm -r obj_dir

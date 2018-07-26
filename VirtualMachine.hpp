#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <queue>
#include "ThreadSafeQueue.hpp"

class VirtualMachine {
public:
	VirtualMachine();
	~VirtualMachine();
	bool check_Main();
	uint32_t step();

	void set_labelmap(std::unordered_map<std::string, unsigned int> labels);
	std::unordered_map<std::string, unsigned int> get_labelmap();

	void setInstructions(std::vector<std::vector<std::string>> instrvect);
	void setInitialPc();
	void move_pc(uint32_t mem);
	void update_pc();
	void error_occured(std::string err);
	void declarations(uint32_t val);
	bool store_word(uint32_t val, uint32_t loc);
	void store_le(uint32_t val);
	void setMem(int times);
	uint32_t search_labelmap(std::string lab);
	uint32_t search_regmap(std::string reg);

	uint32_t read(std::string val);
	bool write_reg(std::string reg, uint32_t val);
	std::string find_regpair(std::string reg);
	bool exec(std::vector<std::string> curInstr);

	bool is_num(const std::string str);
	bool contains(const std::string str, const char c);

	//Arithmetic
	uint32_t add(int32_t r2, int32_t r3);
	uint32_t addu(uint32_t r2, uint32_t r3);
	uint32_t sub(int32_t r2, int32_t r3);
	uint32_t subu(uint32_t r2, uint32_t r3);
	void mult(int32_t r2, int32_t r3);
	void multu(uint32_t r2, uint32_t r3);
	bool div(int32_t r2, int32_t r3);
	bool divu(uint32_t r2, uint32_t r3);

	//Logical
	uint32_t and_l(uint32_t r2, uint32_t r3);
	uint32_t nor_l(uint32_t r2, uint32_t r3);
	uint32_t not_l(uint32_t r2);
	uint32_t or_l(uint32_t r2, uint32_t r3);
	uint32_t xor_l(uint32_t r2, uint32_t r3);

	//Control 
	bool jump(uint32_t location);
	void beq(uint32_t r1, uint32_t r2, uint32_t loc);
	void bne(uint32_t r1, uint32_t r2, uint32_t loc);
	void blt(uint32_t r1, uint32_t r2, uint32_t loc);
	void ble(uint32_t r1, uint32_t r2, uint32_t loc);
	void bgt(uint32_t r1, uint32_t r2, uint32_t loc);
	void bge(uint32_t r1, uint32_t r2, uint32_t loc);

	//Data movement
	uint32_t lw(uint32_t loc);
	void sw(uint32_t r1, uint32_t loc);
	bool mfhi(std::string rd);
	bool mflo(std::string rd);

	//User
	uint32_t print_reg(std::string reg);
	uint32_t print_add(unsigned long addr);
	std::string status();
	std::vector<std::vector<std::string>> instructions;

	//Run and stop with threads
	void run();
	void stop();
	bool isRunning();

private:
	//Memory 
	unsigned char memory[512];
	//Register, label, constant maps
	std::unordered_map<std::string, unsigned int> reg_map;
	std::unordered_map<std::string, unsigned int> label_map;
	std::unordered_map < std::string, unsigned int> constant_map;
	//Various variables
	size_t pc, char_location, pc_max;
	uint32_t r1, r2, r3, mem_ind;
	int r1_s, r2_s, r3_s;
	std::string rd, pc_reg, hi, lo, error, msg;
	std::vector<std::string> curInstr;
	//Thread members
	ThreadSafeQueue<std::string> msgQ;
	std::thread m_thread;
	void run_func();
	bool running;
};


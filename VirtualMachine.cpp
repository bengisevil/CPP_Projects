#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <cctype>
#include "VirtualMachine.hpp"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <thread>

VirtualMachine::VirtualMachine()
{
	msg = "";
	running = false;
	char_location = 999;
	mem_ind = 0;
	pc_max = instructions.size() - 1;
	hi = "$hi";
	lo = "$lo";
	pc_reg = "$pc";
	int i = 0;
	//Initialize all memory locations to 0x00
	while (i < 512)
	{
		memory[i] = 0x00;
		i++;
	}
	//Initialize all registers to 0
	reg_map = { { "$zero", 0 },{ "$0", 0 },{ "$at", 0 },{ "$1", 0 },{ "$v0", 0 },{ "$2", 0 },{ "$v1", 0 },{ "$3", 0 },{ "$a0", 0 },{ "$4", 0 },
	{ "$a1", 0 },{ "$5", 0 },{ "$a2", 0 },{ "$6", 0 },{ "$a3", 0 },{ "$7", 0 },{ "$t0", 0 },{ "$8", 0 },{ "$t1", 0 },{ "$9", 0 },
	{ "$t2", 0 },{ "$10", 0 },{ "$t3", 0 },{ "$11", 0 },{ "$t4", 0 },{ "$12", 0 },{ "$t5", 0 },{ "$13", 0 },{ "$t6", 0 },{ "$14", 0 },
	{ "$t7", 0 },{ "$15", 0 },{ "$s0", 0 },{ "$16", 0 },{ "$s1", 0 },{ "$17", 0 },{ "$s2", 0 },{ "$18", 0 },{ "$s3", 0 },{ "$19", 0 },
	{ "$s4", 0 },{ "$20", 0 },{ "$s5", 0 },{ "$21", 0 },{ "$s6", 0 },{ "$22", 0 },{ "$s7", 0 },{ "$23", 0 },{ "$t8", 0 },{ "$24", 0 },
	{ "$t9", 0 },{ "$25", 0 },{ "$k0", 0 },{ "$26", 0 },{ "$k1", 0 },{ "$27", 0 },{ "$gp", 0 },{ "$28", 0 },{ "$sp", 0 },{ "$29", 0 },
	{ "$fp", 0 },{ "$30", 0 },{ "$ra", 0 },{ "$31", 0 },{ "$pc", 0 },{ "$hi", 0 },{ "$lo", 0 } };

}

VirtualMachine::~VirtualMachine() {}

bool VirtualMachine::check_Main()
{
	uint32_t found = search_labelmap("main");
	if (found == 999)
	{
		error_occured("Error: Main does not exist.");
		return false;
	}
	else
		return true;

}

uint32_t VirtualMachine::step()
{
	size_t temp = pc;
	if (pc < instructions.size()) {
		if (!exec(instructions.at(pc)))
		{
			pc = temp;
		}
	}
	return print_reg("$pc");
}

void VirtualMachine::set_labelmap(std::unordered_map<std::string, unsigned int> labels)
{
	label_map = labels;
}


std::unordered_map<std::string, unsigned int> VirtualMachine::get_labelmap()
{
	return label_map;
}

void VirtualMachine::setInstructions(std::vector<std::vector<std::string>> instrvect)
{
	instructions = instrvect;
}

void VirtualMachine::setInitialPc()
{
	pc = search_labelmap("main");
	move_pc(pc);
}

void VirtualMachine::move_pc(uint32_t mem)
{
	pc = mem;
	write_reg(pc_reg, pc);
}

void VirtualMachine::update_pc()
{
	if (pc < pc_max)
	{
		pc++;
		write_reg(pc_reg, pc);
	}
}

void VirtualMachine::error_occured(std::string err)
{
	error = err;
}

void VirtualMachine::declarations(uint32_t val)
{
	uint8_t *byteArr = (uint8_t*)&val;
	for (uint32_t i = 0; i < sizeof(val); i++)
	{
		memory[mem_ind] = byteArr[i];
		mem_ind++;
	}
}

bool VirtualMachine::store_word(uint32_t val, uint32_t loc)
{
	if (loc > 508)
		return false;
	uint8_t *byteArr = (uint8_t*)&val;
	for (uint32_t i = 0; i < sizeof(val); i++)
	{
		memory[loc] = byteArr[i];
		loc++;
	}
	return true;

}

void VirtualMachine::store_le(uint32_t val)
{
	uint8_t *byteArr = (uint8_t*)&val;
	for (uint32_t i = 0; i < sizeof(val); i++)
	{
		memory[mem_ind] = byteArr[i];
		mem_ind++;
	}
}

void VirtualMachine::setMem(int times)
{
	int temp = 0;
	while (temp < times)
	{
		if (mem_ind < 512)
		{
			memory[mem_ind] = 0x00;
			mem_ind++;
			temp++;
		}
		else
			break;
	}
}

uint32_t VirtualMachine::search_labelmap(std::string lab)
{
	int addr = -1;
	bool found = false;
	auto it = label_map.find(lab);
	if (it != label_map.end())
	{
		found = true;
		addr = it->second;
	}
	if (found)
		return addr;
	else
		return 999;
}

uint32_t VirtualMachine::search_regmap(std::string reg)
{
	int addr = -1;
	auto it = reg_map.find(reg);
	if (it != reg_map.end())
	{
		addr = it->second;
	}
	return addr;
}

uint32_t VirtualMachine::read(std::string x)
{
	uint32_t result = 999;
	if (x[0] == '$')
	{
		result = search_regmap(x);
	}
	else if (is_num(x))
	{
		result = stoi(x);
	}
	else if (x[0] == '(')
	{
		if (x[1] == '$')
		{
			result = search_regmap(x.substr(1, x.length()));
		}
		else
		{
			result = search_labelmap(x.substr(1, (x.length() - 1)));
		}
	}
	else
	{
		result = search_labelmap(x);
	}
	return result;
}

bool VirtualMachine::write_reg(std::string reg, uint32_t val)
{
	uint32_t temp = search_regmap(reg);
	std::string r;
	if (temp != 999)
	{
		auto it = reg_map.find(reg);
		if (it != reg_map.end())
		{
			it->second = val;
			if ((reg != "$pc") && (reg != "$hi") && (reg != "$lo")) {
				r = find_regpair(reg);
				auto it_m = reg_map.find(r);
				if (it_m != reg_map.end())
				{
					it_m->second = val;
				}
			}
			return true;
		}
	}
	return false;
}

std::string VirtualMachine::find_regpair(std::string reg)
{
	std::string pair;
	auto it = reg_map.find(reg);
	if (it != reg_map.end())
	{
		if (reg == "$zero")
		{
			pair = "$0";
		}
		else if (reg == "$at")
		{
			pair = "$1";
		}
		else if (reg == "$v0")
		{
			pair = "$2";
		}
		else if (reg == "$v1")
		{
			pair = "$3";
		}
		else if (reg == "$a0")
		{
			pair = "$4";
		}
		else if (reg == "$a1")
		{
			pair = "$5";
		}
		else if (reg == "$a2")
		{
			pair = "$6";
		}
		else if (reg == "$a3")
		{
			pair = "$7";
		}
		else if (reg == "$t0")
		{
			pair = "$8";
		}
		else if (reg == "$t1")
		{
			pair = "$9";
		}
		else if (reg == "$t2")
		{
			pair = "$10";
		}
		else if (reg == "$t3")
		{
			pair = "$11";
		}
		else if (reg == "$t4")
		{
			pair = "$12";
		}
		else if (reg == "$t5")
		{
			pair = "$13";
		}
		else if (reg == "$t6")
		{
			pair = "$14";
		}
		else if (reg == "$t7")
		{
			pair = "$15";
		}
		else if (reg == "$s0")
		{
			pair = "$16";
		}
		else if (reg == "$s1")
		{
			pair = "$17";
		}
		else if (reg == "$s2")
		{
			pair = "$18";
		}
		else if (reg == "$s3")
		{
			pair = "$19";
		}
		else if (reg == "$s4")
		{
			pair = "$20";
		}
		else if (reg == "$s5")
		{
			pair = "$21";
		}
		else if (reg == "$s6")
		{
			pair = "$22";
		}
		else if (reg == "$s7")
		{
			pair = "$23";
		}
		else if (reg == "$t8")
		{
			pair = "$24";
		}
		else if (reg == "$t9")
		{
			pair = "$25";
		}
		else if (reg == "$k0")
		{
			pair = "$26";
		}
		else if (reg == "$k1")
		{
			pair = "$27";
		}
		else if (reg == "$gp")
		{
			pair = "$28";
		}
		else if (reg == "$sp")
		{
			pair = "$29";
		}
		else if (reg == "$fp")
		{
			pair = "$30";
		}
		else if (reg == "$ra")
		{
			pair = "$31";
		}
		else
		{
			pair = "999";
		}
	}
	return pair;
}


bool VirtualMachine::exec(std::vector<std::string> curInstr)
{
	r1 = 0;
	r2 = 0;
	r3 = 0;
	r1_s = 0;
	r2_s = 0;
	r3_s = 0;
	rd = "";
	uint32_t addr;
	curInstr = instructions.at(pc);
	if (curInstr.at(0) == "add")
	{
		rd = curInstr.at(1);
		r2_s = (int)read(curInstr.at(2));
		r3_s = (int)read(curInstr.at(3));
		r1_s = add(r2_s, r3_s);
		if (!write_reg(rd, r1_s))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "addu")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = addu(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "sub")
	{
		rd = curInstr.at(1);
		r2_s = (int)read(curInstr.at(2));
		r3_s = (int)read(curInstr.at(3));
		r1_s = sub(r2_s, r3_s);
		if (!write_reg(rd, r1_s))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "subu")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = subu(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "mult")
	{
		r1_s = (int)read(curInstr.at(1));
		r2_s = (int)read(curInstr.at(2));
		mult(r1_s, r2_s);
	}
	else if (curInstr.at(0) == "multu")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		multu(r1, r2);
	}
	else if (curInstr.at(0) == "div")
	{
		r1_s = (int)read(curInstr.at(1));
		r2_s = (int)read(curInstr.at(2));
		if (!div(r1_s, r2_s))
		{
			error_occured("Error: Division by 0 is undefined. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "divu")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		if (!divu(r1, r2))
		{
			error_occured("Error: Division by 0 is undefined. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "and")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = and_l(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "nor")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = nor_l(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "or")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = or_l(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "xor")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r3 = read(curInstr.at(3));
		r1 = xor_l(r2, r3);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "not")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		r1 = not_l(r2);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "lw")
	{
		rd = curInstr.at(1);
		if ((curInstr.at(2)[0] != '(') && contains(curInstr.at(2), '('))
		{
			addr = read(curInstr.at(2).substr(char_location, (curInstr.at(2).length() - 2)));
			addr += read(curInstr.at(2).substr(0, char_location));
		}
		else if (curInstr.at(2)[0] == '(')
		{
			addr = read(curInstr.at(2).substr(1, (curInstr.at(2).length() - 2)));
		}
		else
		{
			addr = read(curInstr.at(2).substr(0, (curInstr.at(2).length())));
		}

		r1 = lw(addr);
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
	}
	else if (curInstr.at(0) == "li")
	{
		rd = curInstr.at(1);
		r1 = read(curInstr.at(2));
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
		update_pc();
	}
	else if (curInstr.at(0) == "la")
	{
		rd = curInstr.at(1);
		if ((curInstr.at(2)[0] != '(') && contains(curInstr.at(2), '('))
		{
			addr = read(curInstr.at(2).substr(char_location, (curInstr.at(2).length() - 2)));
			addr += read(curInstr.at(2).substr(0, char_location));
		}
		else if (curInstr.at(2)[0] == '(')
		{
			addr = read(curInstr.at(2).substr(1, (curInstr.at(2).length() - 2)));
		}
		else
		{
			addr = read(curInstr.at(2));
		}
		r1 = addr;
		if (!write_reg(rd, r1))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
		update_pc();
	}
	else if (curInstr.at(0) == "sw")
	{
		r1 = read(curInstr.at(1));
		if ((curInstr.at(2)[0] != '(') && contains(curInstr.at(2), '('))
		{
			addr = read(curInstr.at(2).substr(char_location, (curInstr.at(2).length() - 2)));
			addr += read(curInstr.at(2).substr(0, char_location));
		}
		else if (curInstr.at(2)[0] == '(')
		{
			addr = read(curInstr.at(2).substr(1, (curInstr.at(2).length() - 2)));
		}
		else
		{
			addr = read(curInstr.at(2));
		}
		sw(r1, addr);
	}
	else if (curInstr.at(0) == "move")
	{
		rd = curInstr.at(1);
		r2 = read(curInstr.at(2));
		if (!write_reg(rd, r2))
		{
			error_occured("Error: Invalid register. \n");
			return false;
		}
		update_pc();
	}
	else if (curInstr.at(0) == "mfhi")
	{
		rd = curInstr.at(1);
		mfhi(rd);
	}
	else if (curInstr.at(0) == "mflo")
	{
		rd = curInstr.at(1);
		mflo(rd);
	}
	else if (curInstr.at(0) == "j")
	{
		addr = read(curInstr.at(1));
		jump(addr);
	}
	else if (curInstr.at(0) == "beq")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		beq(r1, r2, addr);
	}
	else if (curInstr.at(0) == "bne")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		bne(r1, r2, addr);
	}
	else if (curInstr.at(0) == "blt")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		blt(r1, r2, addr);
	}
	else if (curInstr.at(0) == "ble")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		ble(r1, r2, addr);
	}
	else if (curInstr.at(0) == "bgt")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		bgt(r1, r2, addr);
	}
	else if (curInstr.at(0) == "bge")
	{
		r1 = read(curInstr.at(1));
		r2 = read(curInstr.at(2));
		addr = read(curInstr.at(3));
		bge(r1, r2, addr);
	}
	else if (curInstr.at(0) == "nop")
	{
		update_pc();
	}
	else
	{
		addr = 0;
	}

	return true;
}

bool VirtualMachine::is_num(const std::string str)
{
	return !str.empty() && std::find_if(str.begin(),
		str.end(), [](char c) { return !std::isdigit(c); }) == str.end();
}

bool VirtualMachine::contains(const std::string str, const char c)
{
	char_location = 999;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == c)
		{
			char_location = i;
			return true;
		}
	}
	return false;
}

uint32_t VirtualMachine::add(int32_t r2, int32_t r3)
{
	int result = r2 + r3;
	if ((r2 > 0) && (r3 > 0) && (result < 0))
		return 999;
	else if ((r2 < 0) && (r3 < 0) && (result > 0))
		return 999;
	else {
		update_pc();
		return result;
	}
}

uint32_t VirtualMachine::addu(uint32_t r2, uint32_t r3)
{
	update_pc();
	return (r2 + r3);
}

uint32_t VirtualMachine::sub(int32_t r2, int32_t r3)
{
	int result = r2 - r3;
	if ((r2 > 0) && (r3 < 0) && (result < 0))
		return 999;
	else if ((r2 < 0) && (r3 > 0) && (result > 0))
		return 999;
	else {
		update_pc();
		return result;
	}


}

uint32_t VirtualMachine::subu(uint32_t r2, uint32_t r3)
{
	update_pc();
	return (r2 - r3);
}

void VirtualMachine::mult(int32_t r2, int32_t r3)
{
	int64_t result = ((int64_t)(r2)*r3);
	write_reg(lo, ((int32_t)result));
	write_reg(hi, result >> 32);
	update_pc();
}

void VirtualMachine::multu(uint32_t r2, uint32_t r3)
{
	uint64_t result = r2*r3;
	write_reg(lo, ((uint32_t)result));
	write_reg(hi, (result >> 32));
	update_pc();
}

bool VirtualMachine::div(int32_t r2, int32_t r3)
{
	if (r3 != 0)
	{
		write_reg(lo, (r2 / r3)); //quotient
		write_reg(hi, (r2%r3)); //remainder
		update_pc();
		return true;
	}
	return false;
}

bool VirtualMachine::divu(uint32_t r2, uint32_t r3)
{
	if (r3 != 0)
	{
		write_reg(lo, (r2 / r3)); //quotient
		write_reg(hi, (r2%r3)); //remainder
		update_pc();
		return true;
	}
	return false;
}

uint32_t VirtualMachine::and_l(uint32_t r2, uint32_t r3)
{
	update_pc();
	return (r2&r3);
}

uint32_t VirtualMachine::nor_l(uint32_t r2, uint32_t r3)
{
	update_pc();
	return ~(r2 | r3);
}

uint32_t VirtualMachine::not_l(uint32_t r2)
{
	update_pc();
	return ~r2;
}

uint32_t VirtualMachine::or_l(uint32_t r2, uint32_t r3)
{
	update_pc();
	return (r2 | r3);
}

uint32_t VirtualMachine::xor_l(uint32_t r2, uint32_t r3)
{
	update_pc();
	return (r2^r3);
}

bool VirtualMachine::jump(uint32_t location)
{
	if (location < pc_max)
	{
		move_pc(location);
		return true;
	}

}

void VirtualMachine::beq(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 == r2)
		move_pc(loc);
	else
		update_pc();
}

void VirtualMachine::bne(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 != r2)
		move_pc(loc);
	else
		update_pc();
}

void VirtualMachine::blt(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 < r2)
		move_pc(loc);
	else
		update_pc();
}

void VirtualMachine::ble(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 <= r2)
		move_pc(loc);
	else
		update_pc();
}

void VirtualMachine::bgt(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 > r2)
		move_pc(loc);
	else
		update_pc();
}

void VirtualMachine::bge(uint32_t r1, uint32_t r2, uint32_t loc)
{
	if (r1 >= r2)
		move_pc(loc);
	else
		update_pc();
}

uint32_t VirtualMachine::lw(uint32_t loc)
{
	uint32_t load;
	//Handlle array bounds
	uint8_t x0 = memory[loc];
	uint8_t x1 = memory[loc + 1];
	uint8_t x2 = memory[loc + 2];
	uint8_t x3 = memory[loc + 3];
	load = x0 + (x1 << 8) + (x2 << 16) + (x3 << 24);
	update_pc();
	return load;
}


void VirtualMachine::sw(uint32_t r1, uint32_t loc)
{
	store_word(r1, loc);
	update_pc();
}

bool VirtualMachine::mfhi(std::string rd)
{
	uint32_t hi_r = read(hi);
	if (!write_reg(rd, hi_r))
		return false;
	update_pc();
	return true;
}

bool VirtualMachine::mflo(std::string rd)
{
	uint32_t lo_r = read(lo);
	if (!write_reg(rd, lo_r))
		return false;
	update_pc();
	return true;
}

uint32_t VirtualMachine::print_reg(std::string reg)
{
	bool found = false;
	auto it = reg_map.find(reg);
	if (it != reg_map.end())
	{
		found = true;
		return it->second;
	}

	if (!found)
	{
		error_occured("Error: Invalid register. \n");
		return 9999;
	}
	return 0;
}

uint32_t VirtualMachine::print_add(unsigned long addr)
{
	if (addr < 511)
	{
		return memory[addr];
	}
	else
	{
		error_occured("Error: Invalid memory reference. \n");
		return 9999;
	}
}

std::string VirtualMachine::status()
{
	return error;
}

void VirtualMachine::run()
{
	m_thread = std::thread(&VirtualMachine::run_func, this);
	running = true;
}

void VirtualMachine::run_func()
{
	int temp = 0;
	while (1)
	{
		if (msgQ.try_pop(msg))
		{
			if (msg == "BREAK")
			{
				break;
			}
		}
		temp = step();	
	}
}

void VirtualMachine::stop()
{
	msgQ.push("BREAK");
	running = false;
	m_thread.join();
}

bool VirtualMachine::isRunning()
{
	return running;
}

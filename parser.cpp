#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <cctype>
#include "parser.hpp"
#include <vector>
#include <algorithm>

Parser::Parser()
	: m_tl(TokenList()), m_it(m_tl.begin())
{
	const_val = 0;
	const_name = " ";
	space = false;
	word = false;
	parse_int = false;
	fixLineNum = false;
	text = false;
	mem_length = 0;
	memref = "";

}

Parser::~Parser()
{
	// do nothing
}

bool Parser::tokenize_s(std::string & str)
{
	std::istringstream iss(str);
	return tokenize(iss);
}

bool Parser::tokenize(std::istream & ins)
{
	m_tl = ::tokenize(ins);
	if (m_tl.size() == 0)
	{
		return false;
	}
	m_it = m_tl.begin();
	return m_tl.back().type() != ERROR;
}

bool Parser::tokenize_file(const std::string& file_name)
{
	std::ifstream ifs(file_name);
	return tokenize(ifs);
}

const std::string Parser::get_error()
{
	return "Error:" + std::to_string(m_it->line()) +
		": " + m_it->value() + "\n";
}
/*
bool Parser::is_char(char c)
{
	// <char>          ::= any printable ASCII character (see std::isprint)
	return std::isprint(static_cast<unsigned char>(c));
}
*/
bool Parser::is_alpha(char c)
{
	// <alpha>         ::= 'a' | 'b' | ... | 'z' | 'A' | 'B' | ... | 'Z' | '_'
	return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_');
}

bool Parser::is_digit(char c)
{
	// <digit>         ::= '0' | '1' | ... | '9'
	return ('0' <= c) && (c <= '9');
}

bool Parser::data_section_specifier()
{
	if ((m_it->type() == STRING) &&
		(m_it->value() == ".data"))
		return true;
	return false;
}

bool Parser::text_section_specifier()
{
	if ((m_it->type() == STRING) &&
		(m_it->value() == ".text"))
	{
		return true;
	}
	return false;
}

/////////////////////////////////////////
//DATA SECTION GRAMMAR HELPER FUNCTIONS
////////////////////////////////////////

bool Parser::parse_string()
{
	TokenList::iterator it_start = m_it;
	if (m_it == m_tl.end())
		goto bail_out;
	// <string>        ::= '"' {<char>} '"'
	if (m_it->type() != STRING_DELIM)
		goto bail_out;
	if ((++m_it)->type() != STRING)
		goto bail_out;
	if ((++m_it)->type() != STRING_DELIM)
		goto bail_out;
	if (m_it != m_tl.end())
		++m_it;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_declaration()
{
	TokenList::iterator it_start = m_it;

	if (m_it == m_tl.end())
		goto bail_out;

	//<declaration>   ::= <constant> EOL | <label> EOL | [<label>] <layout> EOL
	if (parse_constant())
	{
		if (m_it == m_tl.end())
			goto bail_out;
		if (m_it->type() == EOL)
		{
			goto return_true;
		}
		else
			goto bail_out;
	}
	else if (parse_label_data())
	{
		if (m_it == m_tl.end())
			goto bail_out;
		if (m_it->type() == EOL)
			goto return_true;
		else
		{
			if (parse_layout())
			{
				if (m_it->type() == EOL)
					goto return_true;
				else
					goto bail_out;
			}
			else
				goto bail_out;
		}
	}
	else if (parse_layout())
	{
		if (m_it == m_tl.end())
			goto bail_out;
		if (m_it->type() == EOL)
			goto return_true;
		else
			goto bail_out;
	}
	else
		goto bail_out;

return_true:
	if (m_it != m_tl.end())
		m_it++;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_constant()
{
	TokenList::iterator it_start = m_it;
	TokenList::iterator copy = m_it;
	TokenList::iterator copy1 = m_it;


	if (m_it == m_tl.end())
		goto bail_out;
	//<constant>      :: = <alpha>{ <digit> | <alpha> } EQUAL <integer>
	if ((m_it->type() != STRING) || (!is_alpha(m_it->value().at(0))))
		goto bail_out;
	else
	{
		//Store constant name
		const_name = m_it->value();

		if (m_it != m_tl.end())
			m_it++;
		else
			goto bail_out;
		if (m_it->type() == EQUAL)
		{
			m_it++;
			if (!parse_integer())
				goto bail_out;
			//Retrieve const name on success 
			copy = m_it;
			copy--; copy1 = copy;
			const_val = stoi(copy1->value());
			label_map.insert({ const_name, const_val });
		}
		else
			goto bail_out;
	}
	//m_it++;
	return true;

bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_label_data()
{
	TokenList::iterator it_start = m_it;

	if (m_it == m_tl.end())
		goto bail_out;
	//<label>::= <alpha> { <digit> | <alpha> } ':'
	if ((m_it->type() != STRING) || (!is_alpha(m_it->value().at(0)))
		|| !((m_it->value()).back() == ':'))
		goto bail_out;
	//Insert new label with line number
	if (text)
	{
		label_map.insert({ (m_it->value().substr(0, (m_it->value().length() - 1))), instructions.size() });
	}
	else
	{
		label_map.insert({ (m_it->value().substr(0, (m_it->value().length() - 1))), mem_length });
	}
	if (m_it != m_tl.end())
		++m_it;
	if (m_it->type() == EOL)
	{
		++m_it;
		if (m_it == m_tl.end())
			--m_it;
	}
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_layout()
{
	TokenList::iterator it_start = m_it;
	TokenList::iterator copy = m_it;
	TokenList::iterator copy1 = m_it;
	std::string temp = "";

	if (m_it == m_tl.end())
		goto bail_out;
	//<layout> ::= <int_layout> <integer> {',' <integer> } | <string_layout> <string> 
	if (parse_int_layout())
	{
		if (!parse_integer() && !parse_label_txt())
			goto bail_out;
		else
		{
			if (m_it->type() == EOL)
			{
				if (parse_int)
				{
					copy = m_it;
					copy--; copy1 = copy;
					if (word)
					{
						//Store .word declaration in memory
						vm.store_le(stoi(copy1->value()));
						mem_length += 4;
					}
					if (space)
					{
						vm.setMem(stoi(copy1->value()));
						mem_length += stoi(copy1->value());
					}
				}
				return true;
			}
			while (m_it->type() == SEP)
			{
				if (m_tl.end() != m_it)
					m_it++;
				else
					goto bail_out;
				if (!parse_integer() && !parse_label_txt())
					goto bail_out;
				if (m_it == m_tl.end())
					return true;
			}
		}
	}
	else if (parse_string_layout())
	{
		if (!parse_string())
			goto bail_out;
	}
	else
		goto bail_out;

	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_integer()
{
	TokenList::iterator it_start = m_it;
	parse_int = false;
	if (m_it == m_tl.end())
		goto bail_out;
	// <integer>       ::= <digit> { <digit> }
	if (m_it->type() != STRING)
		goto bail_out;
	if (m_it->value().at(0) == '-')
	{
		for (size_t i = 1; i < (m_it->value()).length(); i++)
		{
			if (!is_digit(m_it->value().at(i))) {
				goto bail_out;
			}
		}
	}
	else {
		for (auto c : m_it->value())
		{
			if (!is_digit(c)) {
				goto bail_out;
			}
		}
	}
	if (m_it != m_tl.end())
		++m_it;
	fixLineNum = true;
	parse_int = true;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_string_layout()
{
	TokenList::iterator it_start = m_it;
	if (m_it == m_tl.end())
		goto bail_out;
	// <string layout> ::= '.ascii' | '.asciiz'
	if (m_it->type() != STRING)
		goto bail_out;
	if (m_it->value() != ".ascii" && m_it->value() != ".asciiz")
		goto bail_out;
	if (m_it != m_tl.end())
		++m_it;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_int_layout()
{
	TokenList::iterator it_start = m_it;
	word = false;
	space = false;
	if (m_it == m_tl.end())
		goto bail_out;
	// <int_layout>    ::= '.word' | '.half' | '.byte' | '.space'
	if (m_it->type() != STRING)
		goto bail_out;
	if (m_it->value() != ".word" && m_it->value() != ".half" &&
		m_it->value() != ".byte" && m_it->value() != ".space")
		goto bail_out;
	if (m_it->value() == ".word")
		word = true;
	if (m_it->value() == ".space")
		space = true;
	if (m_it != m_tl.end())
		++m_it;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

/////////////////////////////////////////
//TEXT SECTION GRAMMAR HELPER FUNCTIONS
////////////////////////////////////////

bool Parser::parse_instruction()
{
	TokenList::iterator it_start = m_it;
	if (m_it == m_tl.end())
		goto bail_out;
	//<instruction> ::= [<label>] EOL | [<label:>] <operation> EOL
	if ((m_it->type() != EOL))
	{
		if (!parse_label_data())
		{
			if (!parse_operation())
				goto bail_out;
			else
			{

				if (m_it->type() != EOL)
				{
					goto bail_out;
				}
			}
		}
		else
		{
			if (m_it == m_tl.end())
				goto bail_out;
			if (m_it->value() == "end:")
			{
				++m_it;
				if (m_it == m_tl.end())
					--m_it;
				return true;
			}
			if (!parse_operation())
				goto bail_out;
			else
			{
				if (m_it->type() != EOL)
				{
					goto bail_out;
				}
			}
		}
	}
	else
	{
		if (m_it != m_tl.end())
			++m_it;
		return true;
	}

	if (m_it != m_tl.end())
		++m_it;
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_operation()
{
	TokenList::iterator it_start = m_it;
	if (m_it == m_tl.end())
		goto bail_out;
	//<operation> ::= 'nop' | <op_code> <op_args>
	if (m_it->value() != "\'nop\'")
	{
		if (!parse_opcode())
			goto bail_out;
	}
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_label_txt()
{
	fixLineNum = false;
	TokenList::iterator it_start = m_it;
	if (m_it == m_tl.end())
		goto bail_out;
	//<label>::= <alpha> { <digit> | <alpha> }
	if ((m_it->type() != STRING) || (!is_alpha(m_it->value().at(0)))
		|| ((m_it->value()).back() == ':'))
		goto bail_out;
	if (m_it != m_tl.end()) {
		++m_it;
		fixLineNum = true;
	}
	return true;
bail_out:
	m_it = it_start;
	return false;
}

VirtualMachine * Parser::getVm()
{
	return &vm;
}

bool Parser::isRegister()
{
	TokenList::iterator it_start = m_it;
	std::string temp = " ";
	temp = m_it->value();
	fixLineNum = false;
	/*<register> are defined as char $ followed immediately by
	* a valid num/alphanum sequence */
	//Registers $0 through $31
	if ((temp == "$0") || (temp == "$1")
		|| (temp == "$2") || (temp == "$3")
		|| (temp == "$4") || (temp == "$5")
		|| (temp == "$6") || (temp == "$7")
		|| (temp == "$8") || (temp == "$9")
		|| (temp == "$10") || (temp == "$11")
		|| (temp == "$12") || (temp == "$13")
		|| (temp == "$14") || (temp == "$15")
		|| (temp == "$16") || (temp == "$17")
		|| (temp == "$18") || (temp == "$19")
		|| (temp == "$20") || (temp == "$21")
		|| (temp == "$22") || (temp == "$23")
		|| (temp == "$24") || (temp == "$25")
		|| (temp == "$26") || (temp == "$27")
		|| (temp == "$28") || (temp == "$29")
		|| (temp == "$30") || (temp == "$31"))
		goto return_true;

	//$a, $v, $k and special registers

	if ((temp == "$zero") || (temp == "$at")
		|| (temp == "$v0") || (temp == "$v1")
		|| (temp == "$a0") || (temp == "$a1")
		|| (temp == "$a2") || (temp == "$a3")
		|| (temp == "$k0") || (temp == "$k1")
		|| (temp == "$gp") || (temp == "$sp")
		|| (temp == "$fp") || (temp == "$ra"))
		goto return_true;

	//t-Registers
	if ((temp == "$t0") || (temp == "$t1")
		|| (temp == "$t2") || (temp == "$t3")
		|| (temp == "$t4") || (temp == "$t5")
		|| (temp == "$t6") || (temp == "$t7")
		|| (temp == "$t8") || (temp == "$t9"))
		goto return_true;

	//s-Registers
	if ((temp == "$s0") || (temp == "$s1")
		|| (temp == "$s2") || (temp == "$s3")
		|| (temp == "$s4") || (temp == "$s5")
		|| (temp == "$s6") || (temp == "$s7"))
		goto return_true;

	//Default:
	m_it = it_start;
	return false;

return_true:
	if (m_it != m_tl.end())
		m_it++;
	fixLineNum = true;
	return true;
}

bool Parser::parse_opcode()
{
	TokenList::iterator it_start = m_it;
	TokenList::iterator copy = m_it;
	TokenList::iterator copy1 = m_it;
	std::string r1, r2, r3;

	/*Data Movement instructions*/
	std::vector<std::string> dataMov1 = { "lw", "lh", "lb", "la",
		"sw", "sh", "sb" };
	std::vector<std::string> dataMov2 = { "mfhi", "mflo", "mthi", "mtlo" };

	/*Arithmetic and Logical Instructions*/
	std::vector<std::string> arith_logic = { "add", "addu", "sub", "subu",
		"mul", "mulo", "mulou", "rem", "remu", "and", "nor", "or", "xor" };
	std::vector<std::string> arith2 = { "move" };

	/*Control Instructions*/
	std::vector<std::string> control = { "beq", "bne", "blt", "ble",
		"bgt", "bge" };

	std::vector<std::string> two_regs = { "mult", "multu", "div", "divu",
		"abs", "neg", "negu" };
	//PARSING depending on each case
	std::string target = m_it->value();
	std::vector<std::string> instr;

	if ((std::find(dataMov1.begin(), dataMov1.end(), target) != (dataMov1.end())))
	{
		//'xx'   <register> SEP <memref>
		m_it++;
		if (m_it == m_tl.end())
		{
			m_it = it_start;
			return false;
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
					m_it = it_start;
					return false;
				}
				if (parse_memref())
				{
					instr.emplace_back(target);
					instr.emplace_back(r1);
					instr.emplace_back(memref);
					if (fixLineNum)
					{
						instr.emplace_back(std::to_string(m_it->line() - 1));
					}
					else
						instr.emplace_back(std::to_string(copy1->line()));
					instructions.emplace_back(instr);
					goto return_true;
				}
			}
		}
	}
	if ((std::find(dataMov2.begin(), dataMov2.end(), target) != (dataMov2.end())))
	{
		//'xx' <register>
		m_it++;
		if (m_it == m_tl.end())
		{
			m_it = it_start;
			return false;
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			instr.emplace_back(target);
			instr.emplace_back(r1);
			instr.emplace_back(std::to_string(m_it->line() - 1));
			instructions.emplace_back(instr);
			goto return_true;
		}
	}
	if ((std::find(arith_logic.begin(), arith_logic.end(), target) != (arith_logic.end())))
	{
		//'xx'   <register> SEP <register> SEP <source> 
		m_it++;
		if (m_it == m_tl.end())
		{
			m_it = it_start;
			return false;
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
					m_it = it_start;
					return false;
				}
				if (isRegister())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					if (m_it->type() == SEP)
					{
						m_it++;
						if (m_it == m_tl.end())
						{
							m_it = it_start;
							return false;
						}
						if (isSource()) {
							copy = m_it;
							copy--; copy1 = copy;
							r3 = copy1->value();
							instr.emplace_back(target);
							instr.emplace_back(r1);
							instr.emplace_back(r2);
							instr.emplace_back(r3);
							if (fixLineNum)
							{
								instr.emplace_back(std::to_string(copy->line() - 1));
							}
							else
								instr.emplace_back(std::to_string(m_it->line()));
							instructions.emplace_back(instr);
							goto return_true;
						}
					}
				}
			}
		}
	}
	if ((std::find(two_regs.begin(), two_regs.end(), target) != (two_regs.end())))
	{
		//'xx'   <register> SEP <register> [SEP <source>]
		m_it++;
		if (m_it == m_tl.end())
		{
			m_it = it_start;
			return false;
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
					m_it = it_start;
					return false;
				}
				if (isRegister())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					/*if (m_it->type() == SEP)
					{
						m_it++;
						if (m_it == m_tl.end())
						{
							m_it = it_start;
							return false;
						}
						if (isSource()) {
							copy = m_it;
							copy--; copy1 = copy;
							r3 = copy1->value();
							instr.emplace_back(target);
							instr.emplace_back(r1);
							instr.emplace_back(r2);
							instr.emplace_back(r3);
							if (fixLineNum)
							{
								instr.emplace_back(std::to_string(m_it->line() - 1));
							}
							else
								instr.emplace_back(std::to_string(m_it->line()));
							instructions.emplace_back(instr);
							goto return_true;
						}
					}*/
					if (m_it->type() == EOL)
					{
						instr.emplace_back(target);
						instr.emplace_back(r1);
						instr.emplace_back(r2);
						//if (fixLineNum)
						//{
						//	instr.emplace_back(std::to_string(copy1->line() - 1));
						//}
						//else
						instr.emplace_back(std::to_string(copy1->line() - 1));
						instructions.emplace_back(instr);
						goto return_true;
					}
				}
			}
		}
	}
	if ((std::find(arith2.begin(), arith2.end(), target) != (arith2.end())))
	{
		//'xx'  <register> SEP <register>
		m_it++;
		if (m_it == m_tl.end())
		{
			m_it = it_start;
			return false;
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
					m_it = it_start;
					return false;
				}
				if (isRegister())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					instr.emplace_back(target);
					instr.emplace_back(r1);
					instr.emplace_back(r2);
					instr.emplace_back(std::to_string(m_it->line() - 1));
					instructions.emplace_back(instr);
					goto return_true;
				}
			}
		}
	}
	if ((std::find(control.begin(), control.end(), target) != (control.end())))
	{
		//'xx' <register> SEP <source> SEP <label>
		m_it++;
		if (m_it == m_tl.end())
		{
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			//copy = m_it;
			//r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
				}
				if (isSource())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					if (m_it->type() == SEP)
					{
						m_it++;
						if (m_it == m_tl.end())
						{
						}
						if (parse_label_txt())
						{
							copy = m_it;
							copy--; copy1 = copy;
							r3 = copy1->value();
							instr.emplace_back(target);
							instr.emplace_back(r1);
							instr.emplace_back(r2);
							instr.emplace_back(r3);
							instr.emplace_back(std::to_string(copy1->line() - 1));
							instructions.emplace_back(instr);
							goto return_true;
						}
					}
				}
			}
		}
	}
	if (target == "li")
	{
		//'li'   <register> SEP <immediate>
		m_it++;
		if (m_it == m_tl.end())
		{
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
				}
				if (isImmediate())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					instr.emplace_back(target);
					instr.emplace_back(r1);
					instr.emplace_back(r2);
					instr.emplace_back(std::to_string(m_it->line() - 1));
					instructions.emplace_back(instr);
					goto return_true;
				}
				if (parse_label_txt())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					instr.emplace_back(target);
					instr.emplace_back(r1);
					instr.emplace_back(r2);
					instr.emplace_back(std::to_string(m_it->line() - 1));
					instructions.emplace_back(instr);
					goto return_true;
				}
			}
		}
	}
	if (target == "not")
	{
		//'not' <register> SEP <source>
		m_it++;
		if (m_it == m_tl.end())
		{
		}
		if (isRegister())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			if (m_it->type() == SEP)
			{
				m_it++;
				if (m_it == m_tl.end())
				{
				}
				if (isSource())
				{
					copy = m_it;
					copy--; copy1 = copy;
					r2 = copy1->value();
					instr.emplace_back(target);
					instr.emplace_back(r1);
					instr.emplace_back(r2);
					if (fixLineNum) {
						instr.emplace_back(std::to_string(m_it->line() - 1));
					}
					instructions.emplace_back(instr);
					goto return_true;
				}
			}
		}
	}
	if (target == "j")
	{
		//'j' <label>
		m_it++;
		if (m_it == m_tl.end())
		{
		}
		if (parse_label_txt())
		{
			copy = m_it;
			copy--; copy1 = copy;
			r1 = copy1->value();
			instr.emplace_back(target);
			instr.emplace_back(r1);
			instr.emplace_back(std::to_string(copy1->line()-1));
			instructions.emplace_back(instr);
			goto return_true;
		}
	}
	if (target == "nop")
	{
		m_it++;
		instr.emplace_back(target);
		instr.emplace_back(std::to_string(m_it->line()-1));
		instructions.emplace_back(instr);
		goto return_true;
	}

	//Default Case
	m_it = it_start;
	return false;

return_true:
	return true;
}

bool Parser::parse_memref()
{
	fixLineNum = false;
	TokenList::iterator it_start = m_it;
	TokenList::iterator copy = m_it;
	TokenList::iterator copy1 = m_it;
	std::string offset, ref;
	memref = "";

	//<memref> ::= <label> | <register> | [offset] '(' <register> ')' | <immediate> | [offset] (label) | offset (immediate)
	if (parse_label_txt())
	{
		copy = m_it;
		copy--; copy1 = copy;
		memref = copy1->value();
		fixLineNum = true;
		goto return_true;
	}
	if (isRegister()) {
		if (m_it->type() == EOL)
		{
			copy = m_it;
			copy--; copy1 = copy;
			memref = copy1->value();
			goto return_true;
		}
		goto bail_out;
	}
	else if (parse_integer())
	{
		if (m_it->type() == EOL)
		{
			copy = m_it;
			copy--;
			copy1 = copy;
			memref = copy1->value();
			fixLineNum = true;
			goto return_true;
		}
		copy = m_it;
		copy--; copy1 = copy;
		offset = copy1->value();
		if (m_it->type() == OPEN_PAREN)
		{
			ref = "(";
			m_it++;
			if (isRegister())
			{
				copy = m_it;
				copy--; copy1 = copy;
				ref = ref + copy1->value();
				if (m_it->type() == CLOSE_PAREN) {
					ref = ref + ")";
					m_it++;
					memref = offset + ref;
					fixLineNum = true;
					goto return_true;
				}
			}
			else if (parse_label_txt())
			{
				copy = m_it;
				copy--; copy1 = copy;
				ref = ref + copy1->value();
				if (m_it->type() == CLOSE_PAREN) {
					ref = ref + ")";
					m_it++;
					memref = offset + ref;
					fixLineNum = true;
					goto return_true;
				}
			}
			else
			{
				m_it = it_start;
				return false;
			}
		}
	}
	else if (m_it->type() == OPEN_PAREN)
	{
		ref = "(";
		m_it++;
		if (isRegister())
		{
			if (m_it->type() == CLOSE_PAREN) {
				copy = m_it;
				copy--; copy1 = copy;
				ref = ref + copy1->value() + ")";
				m_it++;
				memref = ref;
				goto return_true;
			}
		}
	}
	else
	{
		m_it = it_start;
		return false;
	}
	//Default
	m_it = it_start;
	return false;
return_true:
	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::isImmediate()
{
	TokenList::iterator it_start = m_it;
	//<immediate> is an integer 
	if ((!parse_integer()))
		goto bail_out;
	return true;

bail_out:
	m_it = it_start;
	return false;
}

bool Parser::isSource()
{
	TokenList::iterator it_start = m_it;
	//<source> is either a <register> or <immediate> or <constant>
	if ((!isRegister()) && (!isImmediate()) && (!parse_label_txt()))
		goto bail_out;

	return true;
bail_out:
	m_it = it_start;
	return false;
}

bool Parser::parse_file(const std::string & file_name)
{
	if (!tokenize_file(file_name))
		return false;
	TokenList::iterator it_start = m_it;
	text = false;
	mem_length = 0;

	if (data_section_specifier())
	{
		m_it++;
		if (m_it != m_tl.end())
			m_it++;
		while (!text_section_specifier())
		{
			if (!parse_declaration())
				goto bail_out;
		}
	}
	if (text_section_specifier())
	{
		text = true;
		m_it++;
		if (m_it != m_tl.end())
			m_it++;
		else
			goto bail_out;
		while (m_it != m_tl.end())
		{
			if (!parse_instruction())
				goto bail_out;
		}
	}

	vm.setInstructions(instructions);
	vm.set_labelmap(label_map);
	return true;
bail_out:
	return false;
}

bool Parser::parse(std::string &str)
{
	if (!tokenize_s(str))
		return false;
	TokenList::iterator it_start = m_it;
	text = false;
	mem_length = 0;

	if (data_section_specifier())
	{
		m_it++;
		if (m_it != m_tl.end())
			m_it++;
		else
			goto bail_out;
		while (!text_section_specifier())
		{
			if (!parse_declaration())
				goto bail_out;
		}
	}
	if (text_section_specifier())
	{
		text = true;
		m_it++;
		if (m_it != m_tl.end())
			m_it++;
		else
			goto bail_out;
		while (m_it != m_tl.end())
		{
			if (!parse_instruction())
				goto bail_out;
		}
	}

	vm.setInstructions(instructions);
	vm.set_labelmap(label_map);
	return true;
bail_out:
	return false;
}
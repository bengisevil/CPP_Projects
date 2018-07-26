#pragma once
#include "lexer.hpp"
#include "VirtualMachine.hpp"

class Parser {
public:
	Parser();
	~Parser();

	// tokenize
	bool tokenize(std::istream & ins);
	bool tokenize_s(std::string & str);
	bool tokenize_file(const std::string & file_name);

	// error
	const std::string get_error();

	// file
	bool parse_file(const std::string & file_name);
	bool parse(std::string &str);

	// data section
	bool parse_int_layout();
	bool parse_string_layout();
	bool parse_integer();
	bool parse_string();
	bool parse_declaration();
	bool parse_constant();
	bool parse_label_data();
	bool parse_layout();

	// text section
	bool parse_instruction();
	bool parse_operation();
	bool isRegister();
	bool parse_opcode();
	bool parse_memref();
	bool isImmediate();
	bool isSource();
	bool parse_label_txt();


	VirtualMachine* getVm();

private:
	//bool is_char(char c);
	bool is_alpha(char c);
	bool is_digit(char c);
	bool data_section_specifier();
	bool text_section_specifier();

private:
	VirtualMachine vm;
	TokenList m_tl;
	TokenList::iterator m_it;
	int const_val, mem_length;
	bool word, space, parse_int, text, fixLineNum;
	std::string memref;
	std::string const_name;
	std::unordered_map<std::string, unsigned int> reg_map;
	std::unordered_map<std::string, unsigned int> label_map;
	std::vector<std::vector<std::string>> instructions;

};
#include "test_config.hpp"
#include "catch.hpp"

#include "parser.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

TEST_CASE("data section", "[parser]") {
	{
		std::string str_true = "\"abc\"";
		std::string str_false = "XXX";
		Parser parser;
		/*
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test00.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test01.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test02.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test03.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test04.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test05.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test06.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test07.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/win/test08.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test00.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test01.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test02.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test03.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test04.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test05.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test06.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test07.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/pass/unix/test08.asm") == true);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test00.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test01.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test02.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test03.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test04.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test05.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test06.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/win/test07.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test01.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test02.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test03.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test04.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test05.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test06.asm") == false);
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/fail/unix/test07.asm") == false);
		*/


		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_string() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_string() == false);
	}

	{
		std::string str_true = "0123456789";
		std::string str_false = "XXX0123456789";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_integer() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_integer() == false);
	}

	{
		std::string str_true = ".ascii";
		std::string str_false = "XXX.ascii";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_string_layout() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_string_layout() == false);
	}

	{
		std::string str_true = ".word";
		std::string str_false = "XXX.word";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_int_layout() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_int_layout() == false);
	}

	{
		std::string str_true = ".word 1, 2, 3, 4, 5, 6, 7, 8";
		std::string str_true1 = ".ascii \"xyz\" ";
		std::string str_true2 = ".word	1024";
		std::string str_false = ".word 1,2,3, \n";
		std::string str_false1 = ".ascii 123 \n";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_layout() == true);

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_layout() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.parse_layout() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_layout() == false);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_layout() == false);

	}

	{
		std::string str_true = "ab2d:";
		std::string str_false = "abc ";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_label_data() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_label_data() == false);
	}

	{
		std::string str_true = "b2 = 4";
		std::string str_false = "2c = x";
		std::string str_false1 = "c = x";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_constant() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_constant() == false);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_constant() == false);

	}
	{
		std::string str_true1 = "b2 = 4 \n";
		std::string str_true2 = "someLabel: \n";
		std::string str_true3 = "someLabel: .ascii \"xyz\" \n";
		std::string str_true4 = ".word 1024 \n";
		std::string str_false1 = "var1:	var2: .word 1024";
		std::string str_false2 = "b2 = x";
		std::string str_false3 = "var1: .ascii ";
		std::string str_false4 = "b2 = 4         ";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_declaration() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.parse_declaration() == true);

		REQUIRE(parser.tokenize_s(str_true3) == true);
		REQUIRE(parser.parse_declaration() == true);

		REQUIRE(parser.tokenize_s(str_true4) == true);
		REQUIRE(parser.parse_declaration() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_declaration() == false);

		REQUIRE(parser.tokenize_s(str_false2) == true);
		REQUIRE(parser.parse_declaration() == false);

		REQUIRE(parser.tokenize_s(str_false3) == true);
		REQUIRE(parser.parse_declaration() == false);

		REQUIRE(parser.tokenize_s(str_false4) == true);
		REQUIRE(parser.parse_declaration() == false);
	}

}

TEST_CASE("text section", "[parser]") {
	{
		std::string str_true = "label1";
		std::string str_false = "1label";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true) == true);
		REQUIRE(parser.parse_label_txt() == true);

		REQUIRE(parser.tokenize_s(str_false) == true);
		REQUIRE(parser.parse_label_txt() == false);
	}

	{
		std::string str_true1 = "label: and	$t2, $t0, $t1 \n";
		std::string str_true3 = "abc: and $t2, $t0, $t7 \n";
		std::string str_true4 = "subu	$t2, $t0, $t1 \n";
		std::string str_false1 = "subu	$t2, $t0, $t122";
		std::string str_false3 = "label1 label2 lw $t1, avar";
		std::string str_false4 = "12345";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_instruction() == true);

		REQUIRE(parser.tokenize_s(str_true3) == true);
		REQUIRE(parser.parse_instruction() == true);

		REQUIRE(parser.tokenize_s(str_true4) == true);
		REQUIRE(parser.parse_instruction() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_instruction() == false);

		REQUIRE(parser.tokenize_s(str_false3) == true);
		REQUIRE(parser.parse_instruction() == false);

		REQUIRE(parser.tokenize_s(str_false4) == true);
		REQUIRE(parser.parse_instruction() == false);

	}

	{
		std::string str_true1 = "\'nop\'";
		std::string str_true2 = "nor	$t2, $t0, $t1";
		std::string str_false1 = "nopwe423";
		std::string str_false2 = "lw ";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_operation() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.parse_operation() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_operation() == false);

		REQUIRE(parser.tokenize_s(str_false2) == true);
		REQUIRE(parser.parse_operation() == false);
	}

	{
		std::string str_true1 = "$zero";
		std::string str_true2 = "$22";
		std::string str_true3 = "$t2";
		std::string str_true4 = "$ra";
		std::string str_false1 = "$";
		std::string str_false2 = "$41";
		std::string str_false3 = "$z2";
		std::string str_false4 = "t2";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.isRegister() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.isRegister() == true);

		REQUIRE(parser.tokenize_s(str_true3) == true);
		REQUIRE(parser.isRegister() == true);

		REQUIRE(parser.tokenize_s(str_true4) == true);
		REQUIRE(parser.isRegister() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.isRegister() == false);

		REQUIRE(parser.tokenize_s(str_false2) == true);
		REQUIRE(parser.isRegister() == false);

		REQUIRE(parser.tokenize_s(str_false3) == true);
		REQUIRE(parser.isRegister() == false);

		REQUIRE(parser.tokenize_s(str_false4) == true);
		REQUIRE(parser.isRegister() == false);

	}

	{
		std::string str_true1 = "beq $t0, $t0, next1";
		std::string str_true2 = "lw $t1, avar";
		std::string str_true3 = "j	end";
		std::string str_true4 = "mflo $t2";
		std::string str_false1 = "beq 3, 4, 5";
		std::string str_false2 = "add $t2, $3, $234 ";
		std::string str_false3 = "\'nop\'";
		std::string str_false4 = "beq $t0, $t0, $a2";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_opcode() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.parse_opcode() == true);

		REQUIRE(parser.tokenize_s(str_true3) == true);
		REQUIRE(parser.parse_opcode() == true);

		REQUIRE(parser.tokenize_s(str_true4) == true);
		REQUIRE(parser.parse_opcode() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_opcode() == false);

		REQUIRE(parser.tokenize_s(str_false2) == true);
		REQUIRE(parser.parse_opcode() == false);

		REQUIRE(parser.tokenize_s(str_false3) == true);
		REQUIRE(parser.parse_opcode() == false);

		REQUIRE(parser.tokenize_s(str_false4) == true);
		REQUIRE(parser.parse_opcode() == false);
	}

	{
		std::string str_true1 = "mylabel";
		std::string str_true2 = "$t2";
		std::string str_true3 = "5($t1)";
		std::string str_true4 = "($a3)";
		std::string str_false1 = "somelabel:";
		std::string str_false2 = "$t2($t1)";
		std::string str_false3 = "2 $t2";
		std::string str_false4 = "$33";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.parse_memref() == true);

		REQUIRE(parser.tokenize_s(str_true2) == true);
		REQUIRE(parser.parse_memref() == true);

		REQUIRE(parser.tokenize_s(str_true3) == true);
		REQUIRE(parser.parse_memref() == true);

		REQUIRE(parser.tokenize_s(str_true4) == true);
		REQUIRE(parser.parse_memref() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.parse_memref() == false);

		REQUIRE(parser.tokenize_s(str_false2) == true);
		REQUIRE(parser.parse_memref() == false);

		REQUIRE(parser.tokenize_s(str_false3) == true);
		REQUIRE(parser.parse_memref() == false);

		REQUIRE(parser.tokenize_s(str_false4) == true);
		REQUIRE(parser.parse_memref() == false);
	}

	{
		std::string str_true1 = "1234";
		std::string str_false1 = "$t2";
		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.isImmediate() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.isImmediate() == false);
	}

	{
		std::string str_true1 = "1234";
		std::string str_false1 = "***\n";

		Parser parser;

		REQUIRE(parser.tokenize_s(str_true1) == true);
		REQUIRE(parser.isSource() == true);

		REQUIRE(parser.tokenize_s(str_false1) == true);
		REQUIRE(parser.isSource() == false);
	}
}

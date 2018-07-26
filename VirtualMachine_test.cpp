#include "test_config.hpp"
#include "catch.hpp"

#include "VirtualMachine.hpp"
#include "parser.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
TEST_CASE("VM", "[Virtual Machine]") {
	{
		//0
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string file = "# simple infinite loop \n	.data\n	.text\nmain: j main\n";
		REQUIRE(parser.parse(file) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		vm->step();
		REQUIRE((vm->print_reg("$pc")) == 0);
		vm->step();
		REQUIRE((vm->print_reg("$pc")) == 0);
		vm->step();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->print_reg("$t2")) == 0);
		REQUIRE((vm->print_reg("$8")) == 0);
		REQUIRE((vm->print_reg("$hi")) == 0);
		REQUIRE((vm->print_reg("$lo")) == 0);

		vm->error_occured("some error happened");
		file = vm->status();
		REQUIRE((vm->print_reg("$234")) == 9999);
		REQUIRE((vm->print_add(24823049)) == 9999);
		std::string temp = "something";
		char t = 'a';
		REQUIRE((vm->contains(temp, t)) == false);
		vm->store_le(43);
		vm->error_occured(temp);
		vm->declarations(32);
		vm->setMem(3);
		vm->read("$2");
		vm->read("LABEL");
		vm->read("4($3)");
		vm->find_regpair(temp);

	}
	{
		//1
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\n	.space 8\nvar1:	.word 1\nvar2: .word -2\n.text\n main:\nla $t0, var1\nlw $t1, 0\nlw $t2, $t0\nlw $t3, 4($t0)\nlw $t4, 4(var1)\nlw $t5, var2\n";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();

		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->print_add(8)) == 0x01);
		REQUIRE((vm->print_add(9)) == 0x00);
		REQUIRE((vm->print_add(12)) == 0xfe);
		REQUIRE((vm->print_add(13)) == 0xff);
		REQUIRE((vm->print_add(14)) == 0xff);
		REQUIRE((vm->print_add(15)) == 0xff);

		vm->step();
		REQUIRE((vm->print_reg("$pc")) == 1);
		REQUIRE((vm->print_reg("$t0")) == 8);
		vm->step();
		REQUIRE((vm->print_reg("$t1")) == 0);
		REQUIRE((vm->print_reg("$pc")) == 2);
		vm->step();
		REQUIRE((vm->print_reg("$t2")) == 1);
		REQUIRE((vm->print_reg("$pc")) == 3);
		vm->step();
		REQUIRE((vm->print_reg("$t3")) == 0xfffffffe);
		REQUIRE((vm->print_reg("$pc")) == 4);
		vm->step();
		REQUIRE((vm->print_reg("$t4")) == 0xfffffffe);
		REQUIRE((vm->print_reg("$pc")) == 5);
		vm->step();
		REQUIRE((vm->print_reg("$t5")) == 0xfffffffe);
		REQUIRE((vm->print_reg("$pc")) == 6);

	}
	{
		//2
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string file = ".data\nr1:	.space 4\n r2: .space 12\n r3: .space 4\n var: .word 7\n.text\n main:\n la $t0, r2\n lw $t1, var\n sw $t1, 0\n sw $t1, $t0\nsw $t1, 4($t0)\nsw $t1, 8(r2)\nsw $t1, r3\n";
		REQUIRE(parser.parse(file) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE(vm->status() == "");
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->print_reg("$t0")) == 4);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->print_reg("$t1")) == 7);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->print_add(0)) == 7);
		REQUIRE((vm->print_add(1)) == 0);
		REQUIRE((vm->print_add(2)) == 0);
		REQUIRE((vm->print_add(3)) == 0);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->print_add(4)) == 7);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->print_add(8)) == 7);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->print_add(12)) == 7);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->print_add(16)) == 7);
		REQUIRE((vm->print_add(17)) == 0);

	}
	{
		//3
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nA = 0\nB = 1\nC = 2\nD = 4\n.text\nmain:\nli $t0, 100\nli $t1, A\nli $t2, B\n	li $t3, C\nli $t4, D";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE(vm->status() == "");
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->print_reg("$t0")) == 100);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->print_reg("$t1")) == 0);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->print_reg("$t2")) == 1);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->print_reg("$t3")) == 2);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->print_reg("$t4")) == 4);
	}
	{
		//4
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = -1234\n.text\nmain:\nli $t0, VALUE\nmove $t1, $t0\nmove $t2, $t1\nmove $t3, $t2\n	move $t4, $t3\n	move $t5, $t4\n	move $t6, $t5\n	move $t7, $t6";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->print_reg("$t0")) == 4294966062);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->print_reg("$t1")) == 4294966062);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->print_reg("$t2")) == 4294966062);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->print_reg("$t3")) == 4294966062);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->print_reg("$t4")) == 4294966062);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->print_reg("$t5")) == 4294966062);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->print_reg("$t6")) == 4294966062);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->print_reg("$t7")) == 4294966062);

	}
	{
		//5
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = -1\nvar:	.word 1\n.text\nmain:\nlw $t0, var\nadd $t1, $t0, VALUE\nadd $t2, $t1, $t0";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->print_reg("$t0")) == 1);
		REQUIRE((vm->print_reg("$t1")) == 0);
	}
	{
		//6
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 12\nvar:	.word 31\n.text\nmain:\nlw $t0, var\n	addu $t1, $t0, VALUE # 31 + 12 = 43\naddu $t2, $t1, $t0 # 43 + 31 = 74\n";
		REQUIRE(parser.parse(temp) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);

	}
	{
		//7
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 2\nvar1:	.word 1\nvar2: .word 12\nvar3: .word -1\n.text\nmain:\nlw $t0, var1\nlw $t1, var2\nlw $t2, var3\nsub $t3, $t0, VALUE # 1 -2 = -1\nsub $t4, $t1, $t0 # 12 -1 = 11\n	sub $t5, $t2, VALUE #  -1 -2 = -3 \n";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
	}
	{
		//8
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 2\nvar1:	.word 1\nvar2: .word 12\nvar3: .word -1\n.text\nmain:\nlw $t0, var1\n	lw $t1, var2\n	lw $t2, var3\n	subu $t3, $t0, VALUE # 1 -2 = -1 = 4294967295\nsubu $t4, $t1, $t0 # 12 -1 = 11\n	subu $t5, $t2, VALUE #  -1 -2 = -3 = 4294967293\n";

		REQUIRE(parser.parse(temp) == true); \
			REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
	}
	{
		//9
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\n	VALUE = 4\n	VALUE2 = -4\nvar1: .word 2\nvar2: .word -2\nvar3: .word 1073741824 #  = 2 ^ 30\n.text\nmain:\nlw $t0, var1\nli $t1, VALUE\nmult $t0, $t1 # 2 * 4 = 8\nmflo $t8\nmfhi $t9\n			lw $t0, var2\n	lw $t1, var1\n	mult $t0, $t1 #  -2 * 2 = -4\n	mflo $t8\n	mfhi $t9\n	lw $t0, var3\nli $t1, VALUE\n	mult $t0, $t1 # 1073741824 * 4 = 4294967296 (overflow)\nmflo $t8\nmfhi $t9\nlw $t0, var3\nli $t1, VALUE2\n	mult $t0, $t1 # 1073741824 * -4 = -4294967296 (overflow)\nmflo $t8\nmfhi $t9\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->step()) == 9);
		REQUIRE((vm->step()) == 10);
		REQUIRE((vm->step()) == 11);
		REQUIRE((vm->step()) == 12);
		REQUIRE((vm->step()) == 13);
		REQUIRE((vm->step()) == 14);
		REQUIRE((vm->step()) == 15);
		REQUIRE((vm->step()) == 16);
		REQUIRE((vm->step()) == 17);
		REQUIRE((vm->step()) == 18);
		REQUIRE((vm->step()) == 19);
		REQUIRE((vm->step()) == 20);
	}
	{
		//10
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 4\nvar1: .word 2\nvar2: .word 1073741824 #  = 2 ^ 30\n.text\n	main:\n	lw $t0, var1\nli $t1, VALUE\nmult $t0, $t1 # 2 * 4 = 8\nmflo $t8\nmfhi $t9\nlw $t0, var2\nli $t1, VALUE\nmultu $t0, $t1 # 1073741824 * 4 = 4294967296 (overflow)\nmflo $t8\nmfhi $t9\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->step()) == 9);
		REQUIRE((vm->step()) == 10);
	}
	{
		//11
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 4\nVALUE2 = -4\nvar1:	.word 2\nvar2: .word -2\nvar3: .word 1073741824 #  = 2 ^ 30\n.text\nmain:\nlw $t0, var1\nli $t1, VALUE\ndiv $t0, $t1 # 2 / 4 = 0 rem 2\nmflo $t8\nmfhi $t9\nlw $t0, var2\nlw $t1, var1\ndivu $t0, $t1 #  -2 / 2 = -1 rem 0\nmflo $t8\nmfhi $t9\nlw $t0, var3\nli $t1, VALUE\ndiv $t0, $t1 # 1073741824 / 4 = 268435456 rem 0\nmflo $t8\nmfhi $t9\nlw $t0, var3\n	li $t1, VALUE2\ndiv $t0, $t1 # 1073741824 / -4 = -268435456 rem 0\n	mflo $t8\n	mfhi $t9\n	# divide by 0\n	div $t0, $0\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->step()) == 9);
		REQUIRE((vm->step()) == 10);
		REQUIRE((vm->step()) == 11);
		REQUIRE((vm->step()) == 12);
		REQUIRE((vm->step()) == 13);
		REQUIRE((vm->step()) == 14);
		REQUIRE((vm->step()) == 15);
		REQUIRE((vm->step()) == 16);
		REQUIRE((vm->step()) == 17);
		REQUIRE((vm->step()) == 18);
		REQUIRE((vm->step()) == 19);
		REQUIRE((vm->step()) == 20);
		REQUIRE((vm->step()) == 20);
	}
	{
		//12
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 4\nvar1: .word 2\nvar2: .word 1073741825 #  = 2 ^ 30 + 1\n.text\n	main:\nlw $t0, var1\nli $t1, VALUE\ndiv $t0, $t1 # 2 / 4 = 0 rem 2\nmflo $t8\nmfhi $t9\nlw $t0, var2\nli $t1, VALUE\ndiv $t0, $t1 # 1073741825 / 4 = 268435456 rem 1\nmflo $t8\nmfhi $t9\n# divide by 0\ndiv $t0, $0\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 6);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->step()) == 9);
		REQUIRE((vm->step()) == 10);
		REQUIRE((vm->step()) == 10);
	}
	{
		//13
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 3\nvar1:	.word 12\nvar2: .word 10\n.text\nmain:\nlw $t0, var1\nlw $t1, var2\nand $t2, $t0, $t1\nand $t2, $t0, VALUE\n";

		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
	}
	{
		//14
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 3\nvar1:	.word 12\nvar2: .word 10\n.text\nmain:\nlw $t0, var1\nlw $t1, var2\nnor $t2, $t0, $t1\nnor $t2, $t0, VALUE\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
	}
	{
		//15
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 3\nvar1:	.word 12\nvar2: .word 10\n.text\nmain:\nlw $t0, var1\nlw $t1, var2\nor $t2, $t0, $t1\nor $t2, $t0, VALUE\n\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
	}
	{
		//16
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 3\nvar1:	.word 12\nvar2: .word 10\n.text\nmain:\nlw $t0, var1\nlw $t1, var2\nxor $t2, $t0, $t1\nxor $t2, $t0, VALUE\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
	}
	{
		//17
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nVALUE = 3\nvar1:	.word 12\nvar2: .word 10\n	.text\n	main:\nlw $t0, var1\nlw $t1, var2\nnot $t2, $t0\nnot $t2, $t1\nnot $t2, VALUE\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
	}
	{
		//18
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\n.text\nmain:	nop\nj next\nnop\nnext:\nnop\nj main\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 0);
	}
	{
		//19
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = ".data\nvar0:	.word 0\nvar1: .word 1\nvar2: .word 2\nvar3: .word 3\n.text\nmain:\nlw $t0, var0\nlw $t1, var1\nlw $t2, var2\nlw $t3, var3\nbeq $t0, $t1, check1\nbeq $t0, $t0, check1\nnop\ncheck1:\n	bne $t0, $t0, check2\n	bne $t0, $t1, check2\nnop\ncheck2:\nbgt $t0, $t0, check3\nbgt $t3, $t1, check3\nnop\ncheck3:\nbge $t0, $t1, check4\nbge $t3, $t2, check4\nnop\ncheck4:\nblt $t3, $t1, check5\nblt $t1, $t3, check5\nnop\ncheck5:\nble $t3, $t1, check6\nble $t3, $t3, check6\nnop\ncheck6:\nnop\nj check6\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);
		REQUIRE((vm->step()) == 2);
		REQUIRE((vm->step()) == 3);
		REQUIRE((vm->step()) == 4);
		REQUIRE((vm->step()) == 5);
		REQUIRE((vm->step()) == 7);
		REQUIRE((vm->step()) == 8);
		REQUIRE((vm->step()) == 10);
		REQUIRE((vm->step()) == 11);
		REQUIRE((vm->step()) == 13);
		REQUIRE((vm->step()) == 14);
		REQUIRE((vm->step()) == 16);
		REQUIRE((vm->step()) == 17);
		REQUIRE((vm->step()) == 19);
		REQUIRE((vm->step()) == 20);
		REQUIRE((vm->step()) == 22);
		REQUIRE((vm->step()) == 23);
		REQUIRE((vm->step()) == 22);
		std::string bad = "????????????";
		REQUIRE((parser.parse(bad)) == true);
		bad = "";
		REQUIRE((parser.parse(bad)) == false);
		REQUIRE((parser.parse_file("testLoop.txt")) == false);

	}
	{
		//20
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		std::string temp = "	# Example program to compute the sum of squares from Jorgensen [2016]\n#---------------------------------------------------------------\n# data declarations\n.data\nn: .word 10\nsumOfSquares: .word 0\n#---------------------------------------------------------------\n# the program\n	.text\nmain:\nlw $t0, n\nli $t1, 1\nli $t2, 0\nsumLoop:\nmult $t1, $t1\nmflo $t3\nadd $t2, $t2, $t3\nadd $t1, $t1, 1\nble $t1, $t0, sumLoop\nsw  $t2, sumOfSquares\nend: \nj end\n\n";
		REQUIRE(parser.parse(temp) == true);
		REQUIRE((vm->check_Main()) == true);
		vm->setInitialPc();
		REQUIRE((vm->print_reg("$pc")) == 0);
		REQUIRE((vm->step()) == 1);

		std::string nomain = "# Example program to compute the sum of squares from Jorgensen[2016]\n#-------------------------------------------------------------- - \n# data declarations\n.data\nn:.word 10\nsumOfSquares : .word 0\n#-------------------------------------------------------------- - \n# the program\n.text\n lw $t0, n\nli $t1, 1\nli $t2, 0\nsumLoop:\nmult $t1, $t1\nmflo $t3\nadd $t2, $t2, $t3\nadd $t1, $t1, 1\nble $t1, $t0, sumLoop\nsw  $t2, sumOfSquares\nend: \nj end\n\n";
		parser.parse(nomain);
		nomain = parser.get_error();
		vm->check_Main();
		vm->get_labelmap();
		vm->store_word(5, 600);
		vm->setMem(600);
		vm->read("(4)");
		vm->write_reg("345345", 4);
		vm->find_regpair("$zero");
		vm->find_regpair("$at");
		vm->find_regpair("$v0");
		vm->find_regpair("$fp");
		vm->find_regpair("$ra");
		vm->find_regpair("$v1");
		vm->find_regpair("$v2");
		vm->find_regpair("$v3");
		vm->find_regpair("$v4");
		vm->find_regpair("$a0");
		vm->find_regpair("$a1");
		vm->find_regpair("$a2");
		vm->find_regpair("$a3");
		vm->find_regpair("$t0");
		vm->find_regpair("$t1");
		vm->find_regpair("$t2");
		vm->find_regpair("$s0");
		vm->find_regpair("$t3");
		vm->find_regpair("$t4");
		vm->find_regpair("$t5");
		vm->find_regpair("$t6");
		vm->find_regpair("$t7");
		vm->find_regpair("$t8");
		vm->find_regpair("$k0");
		vm->find_regpair("$k1");
		vm->find_regpair("$sp");
		vm->find_regpair("$gp");
		vm->find_regpair("$hi");
		vm->find_regpair("$lo");
		vm->find_regpair("$pc");
		vm->find_regpair("$t9");
		vm->find_regpair("$s4");
		vm->find_regpair("$s5");
		vm->find_regpair("$s6");
		vm->find_regpair("$s7");
		vm->find_regpair("$s1");
		vm->find_regpair("$s2");
		vm->find_regpair("$s3");

		std::vector<std::string> curInstr;
		curInstr.emplace_back("multu");
		curInstr.emplace_back("$t2");
		curInstr.emplace_back("$t3");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("divu");
		curInstr.emplace_back("$t2");
		curInstr.emplace_back("$t3");

		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("addu");
		curInstr.emplace_back("$t2222");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("addu");
		curInstr.emplace_back("$t2");
		curInstr.emplace_back("$t332324");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("sub");
		curInstr.emplace_back("$t22222");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("subu");
		curInstr.emplace_back("$t2232");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("subu");
		curInstr.emplace_back("$t2");
		curInstr.emplace_back("$t244323");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		curInstr.clear();;
		curInstr.emplace_back("and");
		curInstr.emplace_back("$t2234234");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("or");
		curInstr.emplace_back("$t2234234");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);
		
		curInstr.clear();;
		curInstr.emplace_back("xor");
		curInstr.emplace_back("$t2234234");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("nor");
		curInstr.emplace_back("$t2234234");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("not");
		curInstr.emplace_back("$t2234234");
		curInstr.emplace_back("$t3");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("lw");
		curInstr.emplace_back("$t2");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("lw");
		curInstr.emplace_back("$t3242342");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("li");
		curInstr.emplace_back("$t232342");
		curInstr.emplace_back("$t3");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("la");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("la");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("2($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("la");
		curInstr.emplace_back("$t9993");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("sw");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("move");
		curInstr.emplace_back("$t3423");
		curInstr.emplace_back("$t2");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("nothing");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("add");
		curInstr.emplace_back("$t234233");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		curInstr.clear();;
		curInstr.emplace_back("addu");
		curInstr.emplace_back("$t234233");
		curInstr.emplace_back("$t3");
		curInstr.emplace_back("($t2)");
		vm->exec(curInstr);

		

		vm->add(4294967294, 4294967294);
		vm->add(-42949674, -429496794);









		

	}
	{
		Parser parser;
		VirtualMachine *vm = parser.getVm();
		REQUIRE(parser.parse_file(TEST_FILE_DIR + "/vm/test01.asm") == true);
	}

}
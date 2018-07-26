#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "parser.hpp"
#include "virtual_machine_gui.hpp"
#include <iomanip>
#include <sstream>
#include <qapplication.h>
#include <qdebug.h>
int main(int argc, char *argv[])
{	
	if ((argc < 2) || (argc > 3)) {
		//extern const char *__progname;
		std::cerr << "Usage: " << "simmips" << " [file name]" << std::endl;
		return EXIT_FAILURE;
	}
	if (argc == 3)
	{
		
		if (std::string(argv[1]) != "--gui")
		{
			
			std::cerr << "Usage: " << "simmips" << " --gui [file name]" << std::endl;
			return EXIT_FAILURE;
		}
		else
		{
			QApplication myapp(argc, argv);
			VirtualMachineGUI gui;
			QString str = QString::fromStdString(argv[2]);
			gui.load(str);
			return myapp.exec();
		}
	}	

	
	// parsing
	Parser parser;
	VirtualMachine *vm = parser.getVm();
	if (!parser.parse_file(argv[1])) {
		std::cerr << "Error: File cannot be processed. \n" << std::endl;
		return EXIT_FAILURE;
	}
	if (vm->check_Main() == false)
	{
		std::cout << "Error: Main doesn't exist. \n";
		return EXIT_FAILURE;
	}
	vm->setInitialPc();
	std::string input = "";
	std::string arg = "";
	unsigned long val = 0;
	uint32_t content = 0;
	std::string smp = "simmips> ";

	while (1)
	{
		std::cout << smp;
		std::cin >> input;
		if (input == "print")
		{
			std::cin >> arg;
			if (!(vm->isRunning()))
			{
				if (arg[0] == '$')
				{
					content = vm->print_reg(arg);
					if (content == 9999)
						std::cout << "Error: Invalid register. \n";
					else
					{
						std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << content << std::endl;
					}
				}
				else if (arg[0] == '&')
				{
					val = strtol(arg.substr(3, arg.length()).c_str(), 0, 16);
					content = vm->print_add(val);
					if (content == 9999)
						std::cout << "Error: Invalid memory address. \n";
					else
					{
						std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << content << std::endl;
					}
				}
				else
				{
					std::cout << "Error: Unknown command. \n";
				}
			}
		}
		else if (input == "step")
		{
			if (!(vm->isRunning())) {
				content = vm->step();
				std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << content << std::endl;
			}
		}
		else if (input == "status")
		{
			if (!(vm->isRunning())) {
				if (!(vm->check_Main()))
					std::cout << "Error: Main does not exist. \n" << std::endl;
			}
		}
		else if (input == "quit")
		{
			if (!(vm->isRunning())) {
				break;
			}
		}
		else if (input == "run")
		{
			if (!(vm->isRunning())) {
				vm->run();
			}
		}
		else if (input == "break")
		{
			if (vm->isRunning()) {
				vm->stop();
			}
		}
		else
		{
			if (!(vm->isRunning())) {
				std::cout << "Error: Unknown command." << std::endl;
			}
		}
	}
	std::cin.get();
	return EXIT_SUCCESS;

}

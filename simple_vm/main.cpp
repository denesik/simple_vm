#include "VirtualMachine.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
	if (argc < 3)
		return -1;

	try
	{
		std::ifstream program_file(argv[1], std::ios::binary);
		std::ifstream input_file(argv[2], std::ios::binary);

		std::ofstream output_file;
		if (argc == 4)
			output_file.open(argv[3], std::ios::binary);
		
		VirtualMachine vm(std::string((std::istreambuf_iterator<char>(program_file)), std::istreambuf_iterator<char>()), input_file, argc == 4 ? output_file : std::cout);

		while (vm.Tick());
	}
	catch (const std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}

	return 0;
}

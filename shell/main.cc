#include <iostream>
#include <fstream>

#include "../brainf.hh"

bool FileExists(std::string p_name) {
	std::ifstream fhnd(p_name);
    
	return fhnd.good();
};

std::string ReadFile(std::string p_fname) {
	std::string fstr = "";
	std::ifstream fhnd(p_fname);

	if (fhnd.is_open()) {
		std::string fln = "";

		while (getline(fhnd, fln))
			fstr += fln + '\n';
	};

	fhnd.close();

	return fstr;
};

bool running = false;

int main(int argc, char* argv[]) {
	BF::BF_Interpreter BFi;

	if (argc > 1) {
		if (not FileExists(argv[1])) {
			std::cout << (std::string)"File '" + argv[1] + "' not found" << std::endl;
		
			return 0;
		};

		if (BFi.Interpret(ReadFile(argv[1])) != BF_INTERPRET_OK) {
			std::cout << BFi.GetErrorMsg() << std::endl;

			return 0;
		};

		return BFi.GetCurrentCell();
	};

	std::cout << "Brainfuck Shell\nBy LordOfTrident." << std::endl;

	running = true;
	while (running) {
		std::string in;
		
		std::cout << "> ";
		std::getline(std::cin, in);

		if (in == "exit") {
			running = false;

			continue;
		};

		if (BFi.Interpret(in) != BF_INTERPRET_OK) {
			std::cout << BFi.GetErrorMsg() << std::endl;

			continue;
		};

		std::cout << std::endl;
	};

	return BFi.GetCurrentCell();	
};

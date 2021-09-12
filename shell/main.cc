#include <iostream>
#include <fstream>

#include "../brainf.hh"

bool FileExists(const std::string& p_name) {
	std::ifstream fhnd(p_name);
    
	return fhnd.good();
};

std::string ReadFile(const std::string& p_fname) {
	std::string fstr = "";
	std::ifstream fhnd(p_fname);

	if (fhnd.is_open()) {
		std::string fln = "";

		while (getline(fhnd, fln))
			fstr += fln + '\n';
		
		fhnd.close();
	};

	return fstr;
};

bool running = false;

int main(int argc, const char* argv[]) {
	BF::BF_Interpreter BFi;

	for (ui8 i = 1; i < argc; ++ i) {
		std::string arg = argv[i];
		
		if (arg[0] == '-') {
			if (arg == "-cellcount") {
				++ i;
				arg = argv[i];

				if (i >= argc)
					continue;

				try {
					BFi.SetCellCount(std::stoi(arg));	
				} catch (...) {
					std::cout << "E: Invalid cell count specified" << std::endl;

					return 0;
				};
			} else if (arg == "-bytesize") {
				++ i;
				arg = argv[i];

				if (i >= argc)
					continue;

				try {
					ui8 ByteCount = std::stoi(arg);
					
					if (ByteCount > 4 or ByteCount == 3)
						std::cout << "W: Byte size can only be 1, 2 or 4, if something else is specified its set to 4" << std::endl;
						
					BFi.SetByteCount(ByteCount);
				} catch (...) {
					std::cout << "E: Invalid byte size specified" << std::endl;

					return 0;
				};
			} else {
				std::cout << "E: Invalid parameter '" + arg + "'" << std::endl;

				return 0;
			};

			continue;
		};
		
		if (not FileExists(arg)) {
			std::cout << (std::string)"E: File '" + arg + "' not found" << std::endl;
		
			return 0;
		};

		try {
			BFi.Interpret(ReadFile(arg));
			
			return BFi.GetCurrentCell();
		} catch (std::exception &Error) {
			std::cout << "\nE:" << Error.what() << std::endl;

			return 1;
		};

		return BFi.GetCurrentCell();
	};

	std::cout << "Brainfuck Shell\nBy LordOfTrident.\nParameters:\n  -cellcount <size>\n  -bytesize <size>\n" << std::endl;

	running = true;
	while (running) {
		std::string in;
		
		std::cout << "> ";
		std::getline(std::cin, in);

		if (in == "exit") {
			running = false;

			continue;
		};

		try {
			BFi.Interpret(in);
		} catch (std::exception &Error) {
			std::cout << "\nE: " << Error.what() << std::endl;
		};
		
		std::cout << std::endl;
	};

	return BFi.GetCurrentCell();	
};

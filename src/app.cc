#include <app.hh>

// public
BF::App::App(word p_cellCount, ui8 p_cellSize):
	m_bfi(p_cellCount, p_cellSize),
	m_exitCode(Ok)
{};

BF::App::App(
	const ui8 p_argc,
	const char* p_argv[],
	word p_cellCount,
	ui8 p_cellSize
):
	m_bfi(p_cellCount, p_cellSize),
	m_exitCode(Ok)
{
	Start(p_argc, p_argv);
};

BF::App::~App() {};

BF::Interpreter &BF::App::GetBFi() {
	return m_bfi;
};

ui8 BF::App::GetExitcode() {
	return m_exitCode;
};

void BF::App::Start(const ui8 p_argc, const char* p_argv[]) {
	std::vector <std::string> files = {};

	try {
		ReadParameters(p_argc, p_argv, files);
	} catch (const BF::Exception &error) {
		std::cerr
			<< ERROR
			<< error.What()
			<< std::endl;
	};

	if (files.empty()) {
		Repl();

		return;
	};

	InterpretFiles(files);
};

void BF::App::ReadParameters(
	const ui8 p_argc,
	const char* p_argv[],
	std::vector <std::string> &p_files
) {
	for (ui8 i = 1; i < p_argc; ++ i) {
		std::string arg = p_argv[i];

		switch (arg[0]) {
		case '-': {
				arg = arg.substr(1);

				if (arg == "h" or arg == "-help") {
					std::cout
						<< "\nUsage: app [Options]"
						<< "Options:\n"
						<< "    -h, --help      Show the usage\n"
						<< "    -v  --version   Show the current version\n"
						<< "    -c, --cellcount Set the amount of cells\n"
						<< "    -s, --cellsize  Set the size of a cell in bytes (1, 2 or 4)"
						<< std::endl;
				} else if (arg == "v" or arg == "-version") {
					std::cout
						<< "Version "
						<< BF_VERSION_MAJOR
						<< "."
						<< BF_VERSION_MINOR
						<< "."
						<< BF_VERSION_PATCH
						<< std::endl;
				} else if (arg == "c" or arg == "-cellcount") {
					if (++ i >= p_argc) {
						m_exitCode = ParamNotFound;

						throw BF::Exception("A number for cellcount expected");
					};

					arg = p_argv[i];

					try {
						m_bfi.SetCellCount(std::stoi(arg));
					} catch (...) {
						m_exitCode = InvalidParamError;

						throw BF::Exception("Invalid cellcount number specified");
					};
				} else if (arg == "s" or arg == "-cellsize") {
					if (++ i >= p_argc) {
						m_exitCode = ParamNotFound;

						throw BF::Exception("A number for cellsize expected");
					};

					arg = p_argv[i];

					try {
						m_bfi.SetCellSize(std::stoi(arg));
					} catch (const BF::InvalidDataException &error) {
						std::cerr
							<< WARN
							<< "Cell size can only be 1, 2 or 4;"
							<< "anything else is taken as 4\n"
							<< "(got "
							<< error.What()
							<< ": "
							<< error.Data()
							<< ")"
							<< std::endl;
					} catch (...) {
						m_exitCode = InvalidParamError;

						throw BF::Exception("Invalid cellsize number specified");
					};
				};
			};

			break;

		// Treat all other arguments as file names
		default: p_files.push_back(arg); break;
		};
	};
};

void BF::App::Repl() {
	std::cout
		<< "Brainfuck REPL v"
		<< BF_VERSION_MAJOR
		<< "."
		<< BF_VERSION_MINOR
		<< "."
		<< BF_VERSION_PATCH
		<< "\nhelp: type in help\n";

	bool running = true;
	while (running) {
		std::string input;

		std::cout << "> ";
		std::getline(std::cin, input);

		// Control commands
		if (input == "help") {
			std::cout
				<< "Commands:\n"
				<< "  help  Show the available commands\n"
				<< "  exit  Exit the shell\n"
				<< "  clear Clear the cells (set their values to 0)\n\n"
				<< "For usage help, use the -h or --help parameter.";
		} else if (input == "exit") {
			running = false;

			std::cout << "Exited." << std::endl;

			continue;
		} else if (input == "clear") {
			m_bfi.ClearCells();

			std::cout << "Cells cleared";
		} else { // Else interpret as brainf code
			try {
				m_bfi.Interpret(input);
			} catch (const BF::RuntimeException &error) {
				std::cerr
					<< ERROR
					<< "At line " << error.Line()
					<< ", position " << error.Position()
					<< ":\n  " << error.What();

				m_exitCode = RuntimeError;
			} catch (const BF::InvalidDataException &error) {
				std::cerr
					<< ERROR
					<< error.What()
					<< "\nData value:\n"
					<< error.Data()
					<< "\n";

				m_exitCode = InvalidDataError;
			} catch (const BF::Exception &error) {
				std::cerr
					<< ERROR
					<< error.What()
					<< "\n";

				m_exitCode = GenericError;
			};
		};

		std::cout << std::endl;
	};
};

void BF::App::InterpretFiles(const std::vector <std::string> &p_files) {
	// Execute all files if multiple were specified in
	// the command line parameters
	for (const std::string& file : p_files) {
		if (not FileExists(file)) {
			std::cerr
				<< ERROR
				<< "File '"
				<< file
				<< "' not found"
				<< std::endl;

			m_exitCode = FileNotFound;
			return;
		};

		try {
			m_bfi.Interpret(ReadFile(file));
		} catch (const BF::RuntimeException &error) {
			std::cerr
				<< ERROR
				<< "At line " << error.Line()
				<< ", position " << error.Position()
				<< ":\n  " << error.What();

			m_exitCode = RuntimeError;
			return;
		} catch (const BF::InvalidDataException &error) {
			std::cerr
				<< ERROR
				<< error.What()
				<< "\nData value:\n"
				<< error.Data()
				<< "\n";

			m_exitCode = InvalidDataError;
			return;
		} catch (const BF::Exception &error) {
			std::cerr
				<< ERROR
				<< error.What()
				<< "\n";

			m_exitCode = GenericError;
			return;
		};
	};
};

// private
bool BF::App::FileExists(const std::string &p_name) {
	std::ifstream fileHandle(p_name);

	return fileHandle.good();
};

std::string BF::App::ReadFile(const std::string& p_fileName) {
	std::string fileContents = "";
	std::ifstream fileHandle(p_fileName);

	if (fileHandle.is_open()) {
		std::string fileLine = "";

		while (std::getline(fileHandle, fileLine))
			fileContents += fileLine + '\n';

		fileHandle.close();

		return fileContents;
	};

	m_exitCode = GenericError;

	throw BF::Exception("Could not open the file '" + p_fileName + "'");
};
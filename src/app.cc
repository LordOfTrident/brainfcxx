#include "app.hh"

// public
BF::App::App(usize p_cellCount, u8 p_cellSize):
	m_bfi(p_cellCount, p_cellSize),
	m_exitCode(Ok)
{};

BF::App::App(
	const u8 p_argc,
	const char* p_argv[],
	usize p_cellCount,
	u8 p_cellSize
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

u8 BF::App::GetExitcode() const {
	return m_exitCode;
};

void BF::App::Start(const u8 p_argc, const char* p_argv[]) {
	std::vector <std::string> files = {};

	bool startRepl = true;

	try {
		startRepl = ReadParameters(p_argc, p_argv, files);
	} catch (const BF::Exception &error) {
		std::cerr
			<< "\nError:\n  "
			<< error.What()
			<< std::endl;
	};

	if (files.empty() and startRepl) {
		Repl();

		return;
	};

	InterpretFiles(files);
};

bool BF::App::ReadParameters(
	const u8 p_argc,
	const char* p_argv[],
	std::vector <std::string> &p_files
) {
	bool startRepl = true;

	for (u8 i = 1; i < p_argc; ++ i) {
		std::string arg = p_argv[i];

		switch (arg[0]) {
		case '-': {
				arg = arg.substr(1);

				if (arg == "h" or arg == "-help") {
					std::cout
						<< "Usage: app [Options]"
						<< "Options:\n"
						<< "    -h, --help      Show the usage\n"
						<< "    -v  --version   Show the current version\n"
						<< "    -c, --cellcount Set the amount of cells\n"
						<< "    -s, --cellsize  Set the size of a cell in bytes (1, 2 or 4)"
						<< std::endl;

					startRepl = false;
				} else if (arg == "v" or arg == "-version") {
					std::cout
						<< "Brainfcxx version "
						<< BF_VERSION_MAJOR
						<< "."
						<< BF_VERSION_MINOR
						<< "."
						<< BF_VERSION_PATCH
						<< std::endl;

					startRepl = false;
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
							<< "\nWarning:\n  Cell size can only be 1, 2 or 4;"
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

	return startRepl;
};

void BF::App::Repl() {
	std::cout
		<< "Brainfcxx v"
		<< BF_VERSION_MAJOR
		<< "."
		<< BF_VERSION_MINOR
		<< "."
		<< BF_VERSION_PATCH
		<< " REPL, type in help to see the commands\n";

	Utils::Input inputHandle;

	bool running = true;
	while (running) {
		std::string input;
		inputHandle.GetInput(input, "> ");

		// Control commands
		if (input == "help") {
			std::cout
				<< "Commands:\n"
				<< "  help  Show the available commands\n"
				<< "  exit  Exit the shell\n"
				<< "  clear Clear the cells (set their values to 0)\n\n"
				<< "For usage help, use the -h or --help parameter."
				<< std::endl;
		} else if (input == "exit") {
			running = false;

			std::cout << "Exited." << std::endl;

			continue;
		} else if (input == "clear") {
			m_bfi.ClearCells();

			std::cout << "Cells cleared" << std::endl;
		} else { // Else interpret as brainf code
			try {
				m_bfi.Interpret(input);
			} catch (const BF::RuntimeException &error) {
				std::cerr
					<< "\nREPL:" << error.Line()
					<< ":" << error.Col()
					<< ": error:\n  "
					<< error.What()
					<< std::endl;

				m_exitCode = RuntimeError;
			} catch (const BF::InvalidDataException &error) {
				std::cerr
					<< "\nREPL: error:\n  "
					<< error.What()
					<< "\nData value:\n"
					<< error.Data()
					<< std::endl;

				m_exitCode = InvalidDataError;
			} catch (const BF::Exception &error) {
				std::cerr
					<< "\nREPL: error:\n  "
					<< error.What()
					<< std::endl;

				m_exitCode = GenericError;
			};
		};

		if (input.find('.') != std::string::npos)
			std::cout << std::endl;
	};
};

void BF::App::InterpretFiles(const std::vector <std::string> &p_files) {
	// Execute all files if multiple were specified in
	// the command line parameters
	for (const std::string& file : p_files) {
		if (not FileExists(file)) {
			std::cerr
				<< "\nerror:\n  "
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
				<< "\n" << file
				<< ":" << error.Line()
				<< ":" << error.Col()
				<< ": error:\n  "
				<< error.What()
				<< std::endl;

			m_exitCode = RuntimeError;
			return;
		} catch (const BF::InvalidDataException &error) {
			std::cerr
				<< "\n" << file
				<< ": error:\n  "
				<< error.What()
				<< "\nData value:\n"
				<< error.Data()
				<< "\n";

			m_exitCode = InvalidDataError;
			return;
		} catch (const BF::Exception &error) {
			std::cerr
				<< "\n" << file
				<< ": error:\n  "
				<< error.What()
				<< "\n";

			m_exitCode = GenericError;
			return;
		};
	};
};

// private
bool BF::App::FileExists(const std::string &p_name) const {
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

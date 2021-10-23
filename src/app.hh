#pragma once

#include <components.inc>

// Console error/warning messages
#define ERROR "\nError:\n  "
#define WARN  "\nWarning:\n  "

using BF::ui8;
using BF::ui16;
using BF::ui32;
using BF::ui64;

using BF::i8;
using BF::i16;
using BF::i32;
using BF::i64;

using BF::word;

namespace BF {
	class App {
	public:
		static const ui8 Ok = 0;
		static const ui8 GenericError = 1;
		static const ui8 RuntimeError = 2;
		static const ui8 FileNotFound = 4;
		static const ui8 InvalidDataError = 8;
		static const ui8 InvalidParamError = 16;
		static const ui8 ParamNotFound = 32;

		App(
			word p_cellCount = BF::Interpreter::CellCountDefault,
			ui8 p_cellSize = BF::Interpreter::CellSizeUi8
		);

		App(
			const ui8 p_argc,
			const char* p_argv[],
			word p_cellCount = BF::Interpreter::CellCountDefault,
			ui8 p_cellSize = BF::Interpreter::CellSizeUi8
		);

		~App();

		BF::Interpreter &GetBFi();

		ui8 GetExitcode();

		void Start(const ui8 p_argc, const char* p_argv[]);
		void Repl(); // Read Eval Print Loop
		void InterpretFiles(const std::vector <std::string> &p_files);

	private:
		bool FileExists(const std::string &p_name);
		std::string ReadFile(const std::string& p_fileName);

		void ReadParameters(
			const ui8 p_argc,
			const char* p_argv[],
			std::vector <std::string> &p_files
		);

		BF::Interpreter m_bfi;

		word m_exitCode;
	}; // class App
}; // namespace BF

#pragma once

#include <config.inc>

#include <components.inc>
#include <types.hh>
#include <utils.hh>

// Console error/warning messages
#define ERROR "\nError:\n  "
#define WARN  "\nWarning:\n  "

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 2
#define APP_VERSION_PATCH 1

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

		ui8 GetExitcode() const;

		void Start(const ui8 p_argc, const char* p_argv[]);
		void Repl(); // Read Eval Print Loop
		void InterpretFiles(const std::vector <std::string> &p_files);

	private:
		bool FileExists(const std::string &p_name) const;
		std::string ReadFile(const std::string& p_fileName);

		bool ReadParameters(
			const ui8 p_argc,
			const char* p_argv[],
			std::vector <std::string> &p_files
		);

		BF::Interpreter m_bfi;

		word m_exitCode;
	}; // class App
}; // namespace BF

#ifndef __APP_HH_HEADER_GUARD__
#define __APP_HH_HEADER_GUARD__

#include <config.inc>

#include <components.inc>
#include <types.hh>
#include <utils.hh>

namespace BF {
	class App {
	public:
		static const u8 Ok = 0;
		static const u8 GenericError = 1;
		static const u8 RuntimeError = 2;
		static const u8 FileNotFound = 4;
		static const u8 InvalidDataError = 8;
		static const u8 InvalidParamError = 16;
		static const u8 ParamNotFound = 32;

		App(
			usize p_cellCount = BF::Interpreter::CellCountDefault,
			u8 p_cellSize = BF::Interpreter::CellSize8b
		);

		App(
			const u8 p_argc,
			const char* p_argv[],
			usize p_cellCount = BF::Interpreter::CellCountDefault,
			u8 p_cellSize = BF::Interpreter::CellSize8b
		);

		~App();

		BF::Interpreter &GetBFi();

		u8 GetExitcode() const;

		void Start(const u8 p_argc, const char* p_argv[]);
		void Repl(); // Read Eval Print Loop
		void InterpretFiles(const std::vector <std::string> &p_files);

	private:
		bool FileExists(const std::string &p_name) const;
		std::string ReadFile(const std::string& p_fileName);

		bool ReadParameters(
			const u8 p_argc,
			const char* p_argv[],
			std::vector <std::string> &p_files
		);

		BF::Interpreter m_bfi;

		usize m_exitCode;
	}; // class App
}; // namespace BF

#endif // __APP_HH_HEADER_GUARD__

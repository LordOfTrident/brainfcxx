/*
 *  Brainfcxx - A C++ Brainfuck interpreter
 *  Version 1.2.0
 *  Github: https://github.com/LordOfTrident/cpp-brainfuck
 *
 *  define BF_DONT_USE_EXCEPTION symbol for the library to
 *  use return codes instead of exceptions
 *
 *  define BF_DONT_USE_BITSHIFT for the library to use
 *  the union method instead of byte shifting method.
 *
 *  Since the size of a cell in bytes is changable (1, 2 or
 *  4), there has to be a system to change this at runtime,
 *  and there are two methods in this library to do so.
 *  Either with a union or with byte shifting. Both are
 *  faster in different cases, but i defaulted the byte
 *  shifting one because it uses only 1 byte of memory
 *  when the cell size is set to 1 byte, unlike the union.
 */

#pragma once

#include <iostream> // std::cout, std::cerr, std::cin, noskipws
#include <string>   // std::string
#include <vector>   // std::vector
#include <cstdint>  // std::int8_t, std::int16_t, std::int32_t, std::int64_t,
                    // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
#include <cstddef>  // size_t
#include <climits>  // ULONG_MAX

#define BF_VERSION_MAJOR 1
#define BF_VERSION_MINOR 4
#define BF_VERSION_PATCH 1

#ifdef BF_DONT_USE_EXCEPTION
#	define BF_OK 0
#	define BF_ERROR -1
#	define BF_ERROR_MISSINGCLOSER -2
#	define BF_ERROR_MISSINGOPENER -3
#endif // BF_DONT_USE_EXCEPTION

namespace BF {
	typedef std::int8_t  i8;
	typedef std::int16_t i16;
	typedef std::int32_t i32;
	typedef std::int64_t i64;

	typedef std::uint8_t  ui8;
	typedef std::uint16_t ui16;
	typedef std::uint32_t ui32;
	typedef std::uint64_t ui64;

	typedef std::size_t word;

#ifndef BF_DONT_USE_EXCEPTION
	// Runtime error classes
	class Exception {
	public:
		Exception():
			m_message("")
		{};

		Exception(const std::string &p_message):
			m_message(p_message)
		{};

		~Exception() {};

		const std::string &What() const {
			return m_message;
		};

	private:
		std::string m_message;
	}; // class Exception

	class RuntimeException: public Exception {
	public:
		RuntimeException():
			Exception(),
			m_line(0),
			m_position(0)
		{};

		RuntimeException(
			const std::string &p_message,
			word p_line,
			word p_position
		):
			Exception(p_message),
			m_line(p_line),
			m_position(p_position)
		{};

		~RuntimeException() {};

		word Line() const {
			return m_line;
		};

		word Position() const {
			return m_position;
		};

	private:
		word m_line;
		word m_position;
	}; // class RuntimeException

	class InvalidDataException: public Exception {
	public:
		InvalidDataException():
			Exception(),
			m_data(0)
		{};

		InvalidDataException(
			const std::string &p_message,
			word p_data
		):
			Exception(p_message),
			m_data(p_data)
		{};

		~InvalidDataException() {};

		word Data() const {
			return m_data;
		};

	private:
		word m_data;
	}; // class InvalidDataException

#endif // not BF_DONT_USE_EXCEPTION
	class Interpreter {
	private:
#ifdef BF_DONT_USE_BITSHIFT
		// Cell union type for the union method
		union CellData {
			CellData(ui8 p_cellSize, ui32 p_value) {
				switch (p_cellSize) {
				case CellSizeUi8: m_ui8 = p_value; break;
				case CellSizeUi16: m_ui16 = p_value; break;
				case CellSizeUi32: m_ui32 = p_value; break;
				};
			};

			ui8 m_ui8;
			ui16 m_ui16;
			ui32 m_ui32;
		};

		// CellType definition for shorter code
		typedef CellData CellType;
#else // not BF_DONT_USE_BITSHIFT
		typedef ui8 CellType;
#endif // BF_DONT_USE_BITSHIFT

	public:
		// Constants for setting the cell size and cell count
		static const ui8 CellSizeUi8 = 1;
		static const ui8 CellSizeUi16 = 2;
		static const ui8 CellSizeUi32 = 4;
		static const ui16 CellCountDefault = 256;

		Interpreter(
			word p_cellCount = CellCountDefault,
			ui8 P_cellSize = CellSizeUi8
		):
			m_cellCount(p_cellCount),
			m_cellSize(P_cellSize),
			m_cellPointer(0),
			// resizing and filling cells with 0, preventing a segfault
			// that could happen when GetCurrentCell is called before
			// Interpret
#ifdef BF_DONT_USE_BITSHIFT
			m_cells(p_cellCount, CellData(m_cellSize, 0))
#else // not BF_DONT_USE_BITSHIFT
			m_cells(p_cellCount, 0)
#endif // BF_DONT_USE_BITSHIFT
		{};

#ifdef BF_DONT_USE_EXCEPTION
		i8 Interpret(const std::string &p_code)
#else // not BF_DONT_USE_EXCEPTION
		void Interpret(const std::string &p_code) {
#endif // BF_DONT_USE_EXCEPTION
			m_cellPointer = 0;

			std::vector <word> loops = {};      // For storing the loop indexes
			std::vector <char> inputCache = {}; // For storing unused input

			word line = 1;
			word codeLength = p_code.length();

			for (word i = 0; i < codeLength; ++ i) {
				switch(p_code[i]) {
				case '\n': ++ line; break;

				case '+': IncrementCurrentCell(); break;
				case '-': DecrementCurrentCell(); break;

				case '>':
					++ m_cellPointer;

					if (m_cellPointer >= m_cellCount)
						m_cellPointer = m_cellCount - 1;

					break;

				case '<':
					-- m_cellPointer;

					if (m_cellPointer >= m_cellCount)
						m_cellPointer = m_cellCount - 1;

					break;

#ifdef BF_DONT_USE_BITSHIFT
				case '.': std::cout << (char)GetCurrentCell(); break;
#else // not BF_DONT_USE_BITSHIFT
				// Only read the first byte in the byte shifting method
				case '.':
					std::cout
						<< (char)m_cells[
							m_cellPointer *
							m_cellSize +
							m_cellSize - 1
						];

					break;
#endif // BF_DONT_USE_BITSHIFT
				case ',':
					if (inputCache.empty()) {
						std::string input;
						std::cin >> std::noskipws >> input;
						std::cin.clear();
						std::cin.ignore(ULONG_MAX, '\n');

						// Set the input to a blank space if just enter was pressed
						if (input == "") {
							input = " ";

							std::cin.ignore();
						};

						// Store the input in a cache
						for (const char &ch : input)
							inputCache.push_back(ch);
					};

					// Get a single char from the input
					SetCurrentCell(inputCache[0]);
					inputCache.erase(inputCache.begin());

					break;

				case '[': {
						bool found = false;

						// Check if the loop already exists
						for (const ui8 &pos : loops)
							if (pos == i) {
								found = true;

								break;
							};

						if (not found) {
							if (not GetCurrentCell()) {
								word temp = i;
								word count = 0;

								// Find the matching loop closer
								while(not found and i < codeLength) {
									++ i;

									switch (p_code[i]) {
									case '[':
										++ count;

										break;

									case ']':
										if (count) {
											-- count;

											break;
										};

										found = true;

										break;
									};
								};

								if (not found)
#ifdef BF_DONT_USE_EXCEPTION
									return BF_ERROR_MISSINGCLOSER;
#else // not BF_DONT_USE_EXCEPTION
									throw RuntimeException("Opened loop not closed", line, temp + 1);
#endif // BF_DONT_USE_EXCEPTION

								break;
							};

							loops.push_back(i);
						};
					};

					break;

				case ']':
					if (loops.empty())
#ifdef BF_DONT_USE_EXCEPTION
						return BF_ERROR_MISSINGOPENER;
#else // not BF_DONT_USE_EXCEPTION
						throw RuntimeException("Loop closer without an opener", line, i + 1);
#endif // BF_DONT_USE_EXCEPTION

					// The loop is exited
					if (not GetCurrentCell()) {
						loops.pop_back();

						break;
					};

					// The loop is continued
					i = loops.back();

					break;

				default: break;
				};
			};
		};

		void ClearCells() {
			for (CellType &cell : m_cells)
#ifdef BF_DONT_USE_BITSHIFT
				cell = CellData(m_cellSize, 0);
#else // not BF_DONT_USE_BITSHIFT
				cell = 0;
#endif // BF_DONT_USE_BITSHIFT
		};

#ifdef BF_DONT_USE_EXCEPTION
		ui64 GetCurrentCell() const {
#else // not BF_DONT_USE_EXCEPTION
		ui32 GetCurrentCell() const {
#endif // BF_DONT_USE_EXCEPTION
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSizeUi8: return m_cells[m_cellPointer].m_ui8;
			case CellSizeUi16: return m_cells[m_cellPointer].m_ui16;
			case CellSizeUi32: return m_cells[m_cellPointer].m_ui32;
			};
#else // not BF_DONT_USE_BITSHIFT
			word pos = m_cellPointer * m_cellSize;

			// Put the bytes together depending on the cell size
			switch (m_cellSize) {
			case CellSizeUi8:
				return (ui8)m_cells[pos];

			case CellSizeUi16:
				return
					((ui16)m_cells[pos] << 8) |
					(ui16)m_cells[pos + 1];

			case CellSizeUi32:
				return
					((ui32)m_cells[pos] << 24) |
					((ui32)m_cells[pos + 1] << 16) |
					((ui32)m_cells[pos + 2] << 8) |
					(ui32)m_cells[pos + 3];
			};
#endif // BF_DONT_USE_BITSHIFT

#ifdef BF_DONT_USE_EXCEPTION
			return BF_ERROR;
#else // not BF_DONT_USE_EXCEPTION
			throw InvalidDataException("Invalid cell size", m_cellSize);
#endif // BF_DONT_USE_EXCEPTION
		};

		void SetCellCount(word p_count) {
			m_cellCount = p_count;
#ifdef BF_DONT_USE_BITSHIFT
			m_cells.resize(m_cellCount, CellData(m_cellSize, 0));
#else // not BF_DONT_USE_BITSHIFT
			m_cells.resize(m_cellCount * m_cellSize, 0);
#endif // BF_DONT_USE_BITSHIFT
		};

		void SetCellSize(ui8 p_count) {
			// Only accept 1, 2 or 4 bytes size
			switch (p_count) {
			case CellSizeUi8: case CellSizeUi16: case CellSizeUi32:
				m_cellSize = p_count;

				break;

			default:
				m_cellSize = CellSizeUi32;

				break;
			};
		};

		std::vector <CellType> &GetCells() {
			return m_cells;
		};

	private:
		void IncrementCurrentCell() {
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSizeUi8: ++ m_cells[m_cellPointer].m_ui8; break;
			case CellSizeUi16: ++ m_cells[m_cellPointer].m_ui16; break;
			case CellSizeUi32: ++ m_cells[m_cellPointer].m_ui32; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			word pos = m_cellPointer * m_cellSize + m_cellSize;

			// A method faster than putting the bytes together,
			// adding 1 and then taking it apart.

			// Simply increment the first byte by 1 and do the
			// same on the next one if the first one overflows
			for (i8 i = m_cellSize; i > 0; -- i) {
				-- pos;

				++ m_cells[pos];

				if (m_cells[pos] != 0)
					return;
			};
#endif // BF_DONT_USE_BITSHIFT
		};

		void DecrementCurrentCell() {
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSizeUi8: -- m_cells[m_cellPointer].m_ui8; break;
			case CellSizeUi16: -- m_cells[m_cellPointer].m_ui16; break;
			case CellSizeUi32: -- m_cells[m_cellPointer].m_ui32; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			word pos = m_cellPointer * m_cellSize + m_cellSize;

			// Similar to IncrementCurrentCell method, but here
			// its the opposite
			for (i8 i = m_cellSize; i > 0; -- i) {
				-- pos;
				-- m_cells[pos];

				if (m_cells[pos] != (ui8)-1)
					return;
			};
#endif // BF_DONT_USE_BITSHIFT
		};

		void SetCurrentCell(ui32 p_value) {
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSizeUi8: m_cells[m_cellPointer].m_ui8 = p_value; break;
			case CellSizeUi16: m_cells[m_cellPointer].m_ui16 = p_value; break;
			case CellSizeUi32: m_cells[m_cellPointer].m_ui32 = p_value; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			word pos = m_cellPointer * m_cellSize;

			// Set the value byte by byte
			switch (m_cellSize) {
			case CellSizeUi8:
				m_cells[pos] = (ui8)p_value;

				break;

			case CellSizeUi16:
				m_cells[pos] = (ui8)((p_value & 0xFF00) >> 8);
				m_cells[pos + 1] = (ui8)(p_value & 0x00FF);

				break;

			case CellSizeUi32:
				m_cells[pos] = (ui8)((p_value & 0xFF000000) >> 24);
				m_cells[pos + 1] = (ui8)((p_value & 0x00FF0000) >> 16);
				m_cells[pos + 2] = (ui8)((p_value & 0x0000FF00) >> 8);
				m_cells[pos + 3] = (ui8)(p_value & 0x000000FF);

				break;
			};
#endif // BF_DONT_USE_BITSHIFT
		};

		word m_cellCount;
		ui8 m_cellSize;
		word m_cellPointer;

		std::vector <CellType> m_cells;
	}; // class Interpreter
}; // namespace BF

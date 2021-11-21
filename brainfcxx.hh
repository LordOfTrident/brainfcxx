/*
 *  Brainfcxx - A C++ Brainfuck interpreter
 *  Version 1.4.2
 *  Github: https://github.com/LordOfTrident/brainfcxx
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

#ifndef __BRAINFCXX_HH_HEADER_GUARD__
#define __BRAINFCXX_HH_HEADER_GUARD__

#include <iostream> // std::cout, std::cerr, std::cin, noskipws
#include <string> // std::string
#include <vector> // std::vector
#include <cstdint> // std::int8_t, std::int16_t, std::int32_t, std::int64_t,
                   // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <cstddef> // std::size_t
#include <climits> // ULONG_MAX

#define BF_VERSION_MAJOR 1
#define BF_VERSION_MINOR 5
#define BF_VERSION_PATCH 2

namespace BF {
	typedef std::int8_t  s8;
	typedef std::int16_t s16;
	typedef std::int32_t s32;
	typedef std::int64_t s64;

	typedef std::uint8_t  u8;
	typedef std::uint16_t u16;
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;

	typedef std::size_t usize;

	class Exception {
	public:
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
		RuntimeException(
			const std::string &p_message,
			usize p_line,
			usize p_col
		):
			Exception(p_message),
			m_line(p_line),
			m_col(p_col)
		{};

		~RuntimeException() {};

		usize Line() const {
			return m_line;
		};

		usize Col() const {
			return m_col;
		};

	private:
		usize m_line;
		usize m_col;
	}; // class RuntimeException

	class InvalidDataException: public Exception {
	public:
		InvalidDataException(
			const std::string &p_message,
			usize p_data
		):
			Exception(p_message),
			m_data(p_data)
		{};

		~InvalidDataException() {};

		usize Data() const {
			return m_data;
		};

	private:
		usize m_data;
	}; // class InvalidDataException

	class Interpreter {
	private:
#ifdef BF_DONT_USE_BITSHIFT
		// Cell union type for the union method
		union CellData {
			CellData(u8 p_cellSize, u32 p_value) {
				switch (p_cellSize) {
				case CellSize8b: m_u8 = p_value; break;
				case CellSize16b: m_u16 = p_value; break;
				case CellSize32b: m_u32 = p_value; break;
				};
			};

			u8 m_u8;
			u16 m_u16;
			u32 m_u32;
		};

		// CellType definition for shorter code
		typedef CellData CellType;
#else // not BF_DONT_USE_BITSHIFT
		typedef u8 CellType;
#endif // BF_DONT_USE_BITSHIFT

	public:
		// Constants for setting the cell size and cell count
		static constexpr const u8 CellSize8b  = 1;
		static constexpr const u8 CellSize16b = 2;
		static constexpr const u8 CellSize32b = 4;
		static constexpr const u16 CellCountDefault = 256;

		Interpreter(
			usize p_cellCount = CellCountDefault,
			u8 p_cellSize = CellSize8b
		):
			m_cellCount(p_cellCount),
			m_cellSize(p_cellSize),
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

		void Interpret(const std::string &p_code) {
			m_cellPointer = 0;
			m_line = 1;
			m_col = 0;

			std::vector <usize> loops = {}; // For storing the loop indexes
			std::vector <char> inputCache = {}; // For storing unused input

			usize codeLength = p_code.length();

			for (usize i = 0; i < codeLength; ++ i) {
				++ m_col;

				switch(p_code[i]) {
				case '\n': ++ m_line; m_col = 0; break;

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

						// Set the input to a blank space
						// if just enter was pressed
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
						for (const u8 &pos : loops)
							if (pos == i) {
								found = true;

								break;
							};

						if (not found) {
							if (not GetCurrentCell()) {
								usize temp = m_col;
								usize count = 0;

								// Find the matching loop closer
								while(not found and i < codeLength) {
									++ i; ++ m_col;

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
									throw RuntimeException(
										"Opened loop not closed",
										m_line, temp
									);

								break;
							};

							loops.push_back(i);
						};
					};

					break;

				case ']':
					if (loops.empty())
						throw RuntimeException(
							"Loop closer without an opener",
							m_line, m_col
						);

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

		u32 GetCurrentCell() const {
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSize8b: return m_cells[m_cellPointer].m_u8;
			case CellSize16b: return m_cells[m_cellPointer].m_u16;
			case CellSize32b: return m_cells[m_cellPointer].m_u32;
			};
#else // not BF_DONT_USE_BITSHIFT
			usize pos = m_cellPointer * m_cellSize;

			// Put the bytes together depending on the cell size
			switch (m_cellSize) {
			case CellSize8b:
				return (u8)m_cells[pos];

			case CellSize16b:
				return
					((u16)m_cells[pos] << 8) |
					(u16)m_cells[pos + 1];

			case CellSize32b:
				return
					(static_cast<u32>(m_cells[pos])     << 24) |
					(static_cast<u32>(m_cells[pos + 1]) << 16) |
					(static_cast<u32>(m_cells[pos + 2]) << 8)  |
					 static_cast<u32>(m_cells[pos + 3]);
			};
#endif // BF_DONT_USE_BITSHIFT

			throw InvalidDataException("Invalid cell size", m_cellSize);
		};

		void SetCellCount(usize p_count) {
			m_cellCount = p_count;
#ifdef BF_DONT_USE_BITSHIFT
			m_cells.resize(m_cellCount, CellData(m_cellSize, 0));
#else // not BF_DONT_USE_BITSHIFT
			m_cells.resize(m_cellCount * m_cellSize, 0);
#endif // BF_DONT_USE_BITSHIFT
		};

		void SetCellSize(u8 p_count) {
			// Only accept 1, 2 or 4 bytes size
			switch (p_count) {
			case CellSize8b: case CellSize16b: case CellSize32b:
				m_cellSize = p_count;

				break;

			default:
				m_cellSize = CellSize32b;

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
			case CellSize8b:  ++ m_cells[m_cellPointer].m_u8; break;
			case CellSize16b: ++ m_cells[m_cellPointer].m_u16; break;
			case CellSize32b: ++ m_cells[m_cellPointer].m_u32; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			usize pos = m_cellPointer * m_cellSize + m_cellSize;

			// A method faster than putting the bytes together,
			// adding 1 and then taking it apart.

			// Simply increment the first byte by 1 and do the
			// same on the next one if the first one overflows
			for (s8 i = m_cellSize; i > 0; -- i) {
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
			case CellSize8b:  -- m_cells[m_cellPointer].m_u8; break;
			case CellSize16b: -- m_cells[m_cellPointer].m_u16; break;
			case CellSize32b: -- m_cells[m_cellPointer].m_u32; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			usize pos = m_cellPointer * m_cellSize + m_cellSize;

			// Similar to IncrementCurrentCell method, but here
			// its the opposite
			for (s8 i = m_cellSize; i > 0; -- i) {
				-- pos;
				-- m_cells[pos];

				if (m_cells[pos] != (u8)-1)
					return;
			};
#endif // BF_DONT_USE_BITSHIFT
		};

		void SetCurrentCell(u32 p_value) {
#ifdef BF_DONT_USE_BITSHIFT
			switch (m_cellSize) {
			case CellSize8b:  m_cells[m_cellPointer].m_u8 = p_value; break;
			case CellSize16b: m_cells[m_cellPointer].m_u16 = p_value; break;
			case CellSize32b: m_cells[m_cellPointer].m_u32 = p_value; break;
			};
#else // not BF_DONT_USE_BITSHIFT
			usize pos = m_cellPointer * m_cellSize;

			// Set the value byte by byte
			switch (m_cellSize) {
			case CellSize8b:
				m_cells[pos] = static_cast<u8>(p_value);

				break;

			case CellSize16b:
				m_cells[pos] =     static_cast<u8>((p_value & 0xFF00) >> 010);
				m_cells[pos + 1] = static_cast<u8> (p_value & 0x00FF);

				break;

			case CellSize32b:
				m_cells[pos] =     static_cast<u8>((p_value & 0xFF000000) >> 030);
				m_cells[pos + 1] = static_cast<u8>((p_value & 0x00FF0000) >> 020);
				m_cells[pos + 2] = static_cast<u8>((p_value & 0x0000FF00) >> 010);
				m_cells[pos + 3] = static_cast<u8> (p_value & 0x000000FF);

				break;
			};
#endif // BF_DONT_USE_BITSHIFT
		};

		usize m_line;
		usize m_col;

		usize m_cellCount;
		u8 m_cellSize;
		usize m_cellPointer;

		std::vector <CellType> m_cells;
	}; // class Interpreter
}; // namespace BF

#endif // __BRAINFCXX_HH_HEADER_GUARD__

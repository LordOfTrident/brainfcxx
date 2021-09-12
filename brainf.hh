#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;

typedef size_t word;

namespace BF {
	class BF_Interpreter {
	public: 
		BF_Interpreter():
			CellCount(256),
			ByteCount(1)
		{};

		void Interpret(const std::string& p_Code) {
			CellPointer = 0;
			Cells.resize(CellCount * ByteCount, 0);
			Loops = {};
			Input = {};

			for (word i = 0; i < p_Code.length(); ++ i) {
				switch(p_Code[i]) {
					case '+': {
						SetCurrentValue(GetCurrentValue() + 1);

						break;
					};
					
					case '-': {
						SetCurrentValue(GetCurrentValue() - 1);

						break;
					};

					case '>': {
						++ CellPointer;
						
						if (CellPointer >= CellCount)
							CellPointer = CellCount - 1;

						break;
					};
					
					case '<': {
						-- CellPointer;
						
						if (CellPointer >= CellCount)
							CellPointer = CellCount - 1;

						break;
					};

					case '.': {
						std::cout << (char)GetCurrentValue();

						break;
					};
					
					case ',': {
						if (Input.size() == 0) {
							std::string in;
							std::cin >> std::noskipws >> in;
							std::cin.ignore(1000, '\n');
							std::cin.clear();

							if (in == "") {
								in = " ";
								
								std::cin.ignore();
							};

							for (ui32 j = 0; j < in.length(); ++ j)
								Input.push_back(in[j]);
						};

						SetCurrentValue(Input[0]);
						Input.erase(Input.begin() + 0);

						break;
					};

					case '[': {
						bool found = false;
						
						for (ui32 j = 0; j < Loops.size(); ++ j)
							if (Loops[j] == i) {
								found = true;
								
								break;
							};

						if (not found) {
							if (not GetCurrentValue()) {
								word Previ = i;
								ui32 Count = 0;
	
								while (not found and i < p_Code.size()) {
									++ i;
									
									switch (p_Code[i]) {
										case '[': {
											++ Count;
											
											break;	
										};
										
										case ']': {
											if (Count) {
												-- Count;

												break;
											};
											
											found = true;
											
											break;
										};
									};
								};
	
								if (not found)
									throw std::runtime_error("Loop opened at " + std::to_string(Previ) + " not closed");

								break;
							};
							
							Loops.push_back(i);
						};

						break;
					};

					case ']': {
						if (Loops.size() == 0)
							throw std::runtime_error("Loop closer at " + std::to_string(i) + " without an opener");
						
						if (not GetCurrentValue()) {
							Loops.pop_back();

							break;
						};

						i = Loops[Loops.size() - 1];

						break;
					};

					default: {
						break;
					};
				};
			};
		};

		ui8 GetCurrentCell() {
			return GetCurrentValue();
		};

		void SetCellCount(const ui32& p_Count) {
			CellCount = p_Count;
		};

		void SetByteCount(const ui8& p_Count) {
			switch (p_Count) {
				case 1: case 2: case 4: {
					ByteCount = p_Count;

					break;
				};

				default: {
					ByteCount = 4;

					break;
				};
			};
		};
        
	private:
		ui32 GetCurrentValue() {
			ui32 Pos = CellPointer * ByteCount;
			
			switch (ByteCount) {
				case 1: {
					return (ui8)Cells[Pos];

					break;
				};

				case 2: {
					return
						((ui16)Cells[Pos] << 8) |
						(ui16)Cells[Pos + 1];

					break;
				};

				default: case 4: {
					return 
						((ui32)Cells[Pos] << 24) | 
						((ui32)Cells[Pos + 1] << 16) | 
						((ui32)Cells[Pos + 2] << 8) |
						(ui32)Cells[Pos + 3];

					break;	
				};
			};
		};

		void SetCurrentValue(const ui32& p_Value) {
			ui32 Pos = CellPointer * ByteCount;
			
			switch (ByteCount) {
				case 1: {
					Cells[Pos] = (ui8)p_Value;

					break;
				};

				case 2: {
					Cells[Pos] = (ui8)((p_Value & 0xFF00) >> 8);
					Cells[Pos + 1] = (ui8)(p_Value & 0x00FF);

					break;
				};

				default: case 4: {
					Cells[Pos] = (ui8)((p_Value & 0xFF000000) >> 24);
					Cells[Pos + 1] = (ui8)((p_Value & 0x00FF0000) >> 16);
					Cells[Pos + 2] = (ui8)((p_Value & 0x0000FF00) >> 8);
					Cells[Pos + 3] = (ui8)(p_Value & 0x000000FF);

					break;	
				};
			};
		};

		ui32 CellCount;
		ui8 ByteCount;
		
		std::vector <ui8> Cells;
		ui32 CellPointer;
		
		std::vector <ui32> Loops;
		std::vector <i8> Input;
	};
};

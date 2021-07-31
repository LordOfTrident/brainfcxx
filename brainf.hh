#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#define BF_INTERPRET_OK 0
#define BF_INTERPRET_ERROR 1

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;

namespace BF {
	class BF_Interpreter {
	public: 
		BF_Interpreter():
			CellCount(256)
		{};

		i8 Interpret(std::string p_Code) {
			ErrorMsg = "";
			CellPointer = 0;
			Cells = {};
			Loops = {};
			Input = {};

			for (ui32 i = 0; i < CellCount; ++ i)
				Cells.push_back(0);

			for (ui32 i = 0; i < p_Code.length(); ++ i) {
				switch(p_Code[i]) {
					case '+': {
						++ Cells[CellPointer];

						break;
					};
					
					case '-': {
						-- Cells[CellPointer];

						break;
					};

					case '>': {
						++ CellPointer;

						break;
					};
					
					case '<': {
						-- CellPointer;

						break;
					};

					case '.': {
						std::cout << (i8)Cells[CellPointer];

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

						Cells[CellPointer] = Input[0];
						Input.erase(Input.begin() + 0);

						break;
					};

					case '[': {
						bool found = false;
						for (ui32 j = 0; j < Loops.size(); ++ j)
							if (Loops[j] == i)
								found = true;

						if (found)
							break;
						
						Loops.push_back(i);

						break;
					};

					case ']': {
						if (not Cells[CellPointer]) {
							Loops.pop_back();

							break;
						};

						i = Loops[Loops.size() - 1];

						break;
					};
				};
			};

			if (Loops.size() != 0) {
				ErrorMsg = "Loop opened at " + std::to_string(Loops[Loops.size() - 1]) + " not closed";

				return BF_INTERPRET_ERROR;
        	};
        	
			return BF_INTERPRET_OK;
		};

		ui8 GetCurrentCell() {
			return CellPointer > Cells.size()? 0 : Cells[CellPointer];
		};

		std::string GetErrorMsg() {
			return ErrorMsg;
		};

		void SetCellCount(ui32 p_Count) {
			CellCount = p_Count;
		};
        
	private:
		std::string ErrorMsg;

		ui32 CellCount;
		std::vector <i32> Cells;
		ui8 CellPointer;
		
		std::vector <ui32> Loops;
		std::vector <i8> Input;
	};
};

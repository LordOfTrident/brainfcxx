BinName = app
SourceFolder = ./src/
BinFolder = ./bin/
Include = ./

Extension = .cc
ExtensionHeader = .hh
ExtensionInclude = .inc

SourceFiles = ${SourceFolder}main${Extension} ${SourceFolder}app${Extension}
HeaderFiles = ${Include}brainfcxx${ExtensionHeader} ${SourceFolder}app${ExtensionHeader} ${SourceFolder}components${ExtensionInclude}
AllFiles = ${SourceFIles} ${HeaderFiles}

Compiler = g++
CxxVersion = c++17

ifeq (${OS},Windows_NT)
	Print = echo
	Msg_CreatingBinFolder = "* CMPL: Creating ${BinFolder}\n"
	Msg_Compiling = "* CMPL: Compiling the shell...\n"
	Msg_Compiled = "* CMPL: Compiled successfully\n"

	Msg_Copying = "* INST: Installation is for linux only, sorry :("
	Msg_Copied = "* INST: Copied successfully\n"

	MakeBinFolder = if not exist "${BinFolder}" mkdir ${BinFolder}
	CompilerPath = ./
else
	Print = printf
	Msg_CreatingBinFolder = "\e[0;36m* CMPL: \e[0;33mCreating ${BinFolder}\n\e[0m"
	Msg_Compiling = "\e[0;36m* CMPL: \e[0;33mCompiling the shell...\n\e[0m"
	Msg_Compiled = "\e[0;36m* CMPL: \e[0;32mCompiled successfully\n"

	Msg_Copying = "\e[0;36m* INST: \e[0;33mCopying ${BinFolder}${BinName} to /usr/bin as bfcxx...\n\e[0m"
	Msg_Copied = "\e[0;36m* INST: \e[0;32mCopied successfully\n"

	MakeBinFolder = mkdir -p ${BinFolder}
	CompilerPath = /usr/bin/${Compiler}
endif

compile: ${AllFiles} ${CompilerPath}
	@${Print} ${Msg_CreatingBinFolder}
	@${MakeBinFolder}

	@${Print} ${Msg_Compiling}
	@${Compiler} -I${Include} -I${SourceFolder} ${SourceFiles} -o ${BinFolder}app -O3 -Wall -std=${CxxVersion}

	@${Print} ${Msg_Compiled}

install: ${BinFolder}${BinName}
	@${Print} ${Msg_Copying}
	@sudo cp ${BinFolder}${BinName} /usr/bin/bfcxx
	@${Print} ${Msg_Copied}

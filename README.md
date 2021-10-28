<p align="center">
	<img width="100px" src="assets/logo.png"/>
	<h1 align="center">brainfcxx</h2>
	<p align="center">A brainf+ck interpreter written in C++</p>
</p>
<p align="center">
	<a href="./LICENSE">
		<img alt="License" src="https://img.shields.io/badge/license-GPL-blue?color=7aca00"/>
	</a>
	<a href="https://github.com/LordOfTrident/brainfcxx/issues">
		<img alt="Issues" src="https://img.shields.io/github/issues/LordOfTrident/brainfcxx?color=0088ff"/>
	</a>
	<a href="https://github.com/LordOfTrident/brainfcxx/pulls">
		<img alt="GitHub pull requests" src="https://img.shields.io/github/issues-pr/LordOfTrident/brainfcxx?color=0088ff"/>
	</a>
	<br><br><br>
</p>

Brainfuck is a programming language created in 1993 by Urban Müller, you can read more on it [here](https://en.wikipedia.org/wiki/Brainfuck)

## Features
- Changable cells count
- Changable cell size
- Nested loops support
- Interprets all files in parameters
- Last cells value used for the exitcode
- A REPL when no files were provided

## Usage
The entire interpreter is in a single header file `brainfcxx.hh`. You can use it in your project if you want. Use the `-h` or `--help` parameters to show the usage. If you dont provide any files in the command line parameters, the REPL start automatically.

## Make
Use `make all` to see all the make targets.

### Compiling
Run `make` or `make compile` which will create a binary in the `bin/` folder.

> If you want to change the compiler make uses, set the `CXX` variable. Example: `make CXX=g++`

### Installing
(This step is Linux-only)
To install the interpreter, run `make install` and then you can use `bfcxx` anywhere.

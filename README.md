# Brainfuck
Brainfuck interpreter written in C++

<img src="pic/logo.png"/>

Brainfuck is a programming language created in 1993 by Urban Müller, you can read more on it here https://en.wikipedia.org/wiki/Brainfuck

## Compiling and running the shell
#### Linux
Run `compile` as bash in the terminal:
```sh
bash compile
```
If you dont want it to check for g++, you can use the `-nc` parameter.
This will generate an executable in bin folder, run it:
```sh
./bin/app
```
#### Windows
First make sure you have g++ installed (Command Prompt will not recognize the `g++` command if you dont) and run the following commands:
```bat
mkdir bin
g++ -g shell/main.cc -o bin/app -O3 -s -Wall -std=c++17;
```
then run it:
```bat
cd bin
app
```

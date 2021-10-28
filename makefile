# Names
N_APP = bfcxx

# Files
F_SRC = \
	src/main.cc\
	src/app.cc\
	src/utils.cc

F_HEADER = \
	brainfcxx.hh\
	src/app.hh\
	src/utils.hh\
	src/types.hh\
	src/components.inc\
	src/platform.inc\
	src/config.inc

F_ALL = ${F_SRC} ${F_HEADER}

# Compiler related
CXX = g++
CXX_VER = c++17
CXX_FLAGS = \
	-O3\
	-Wall\
	-std=${CXX_VER}\
	-I./src\
	-I./\
	-o ./bin/app

# Config
UTILS_USE_GNU_READLINE = false

ifeq (${OS}, Windows_NT)
	CREATE_BIN_DIRECTORY = if not exist "./bin" mkdir ${D_BIN}
	CLEAN = del ./bin/app
	INSTALL = echo Install is Linux-only
else
	UNAME_S := $(shell uname -s)
    ifeq (${UNAME_S}, Linux)
    	UTILS_USE_GNU_READLINE = true
    endif

    ifeq (${UTILS_USE_GNU_READLINE}, true)
    	CXX_FLAGS += -lreadline
    endif

	CREATE_BIN_DIRECTORY = mkdir -p ./bin
	CLEAN = rm ./bin/app && rm /usr/bin/${N_APP}
	INSTALL = \
		echo Installing... &&\
		cp ./bin/app /usr/bin/${N_APP} &&\
		echo Installed succesfully
endif

compile: ${F_ALL}
	@${CREATE_BIN_DIRECTORY}
	@echo Created ./bin/

	@echo Compiling...
	@${CXX} ${F_SRC} ${CXX_FLAGS}
	@echo Compiled successfully

install: ./bin/app
	@${INSTALL}

clean:
	@echo Cleaning...
	@${CLEAN}
	@echo Cleaned succesfully...

all:
	@echo compile - Compiles the source
	@echo install - Copies the binary in /usr/bin !Linux only!
	@echo clean - Removes built files

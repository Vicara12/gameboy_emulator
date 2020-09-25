
OPTIONS = -D_GLIBCXX_DEBUG -O2 -Wall -Werror -Wno-unused-parameter -Wextra\
 -Wno-sign-compare -std=c++11

ALL_O = main.o memory.o arithmetic_instructions.o instructions.o\
 load_instructions.o other_instructions.o shift_instructions.o cpu.o\
 instruction_set_vec.o

EXE_NAME = gameboi.exe

all: $(EXE_NAME)

$(EXE_NAME): $(ALL_O)
	g++ $(OPTIONS) -o $(EXE_NAME) $(ALL_O)

main.o: main.cpp
	g++ $(OPTIONS) -c main.cpp

memory.o: memory.cpp memory.h bootrom.h
	g++ $(OPTIONS) -c memory.cpp

cpu.o: cpu.h cpu.cpp instruction_set_vec.h memory.h
	g++ $(OPTIONS) -c cpu.cpp

instructions.o: instructions.h instructions.cpp
	g++ $(OPTIONS) -c instructions.cpp

instruction_set_vec.o: instructions.h arithmetic_instructions.h load_instructions.h\
 other_instructions.h shift_instructions.h instruction_set_vec.h instruction_set_vec.cpp
	g++ $(OPTIONS) -c instruction_set_vec.cpp

arithmetic_instructions.o: arithmetic_instructions.h arithmetic_instructions.cpp instructions.h
	g++ $(OPTIONS) -c arithmetic_instructions.cpp

load_instructions.o: load_instructions.h load_instructions.cpp instructions.h
	g++ $(OPTIONS) -c load_instructions.cpp

other_instructions.o: other_instructions.h other_instructions.cpp instructions.h
	g++ $(OPTIONS) -c other_instructions.cpp

shift_instructions.o: shift_instructions.h shift_instructions.cpp instructions.h
	g++ $(OPTIONS) -c shift_instructions.cpp

clean:
	rm $(ALL_O) $(EXE_NAME)

git:
	git add *.cpp *.h bootrom.txt Makefile


OPTIONS = -D_GLIBCXX_DEBUG -O2 -Wall -Wextra -Werror -Wno-sign-compare -std=c++11

ALL_O = main.o memory.o

EXE_NAME =gameboi.exe

all: $(EXE_NAME)

gameboi.exe: main.o memory.o
	g++ $(OPTIONS) -o $(EXE_NAME) $(ALL_O)

main.o: main.cpp
	g++ $(OPTIONS) -c main.cpp

memory.o: memory.cpp memory.h bootrom.h
	g++ $(OPTIONS) -c memory.cpp

clean:
	rm $(ALL_O) $(EXE_NAME)

git:
	git add *.cpp *.h bootrom.txt Makefile

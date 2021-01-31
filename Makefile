
# compiler options
OPTIONS = -D_GLIBCXX_DEBUG -O2 -Wall -Werror -Wno-unused-parameter -Wextra\
           -Wno-sign-compare -std=c++11

EXE_NAME = gameboi.exe

SFML_COMP = -lsfml-graphics -lsfml-window -lsfml-system


################################################################################

# all object files
ALL_CPP := $(shell ls src/*.cpp)
TEMP    := $(subst src/,build/,$(ALL_CPP))
ALL_O   := $(subst .cpp,.o,$(TEMP))


-include $(subst src,dep,$(@:.o=.d))


.PHONY: clean git count

# make instructions

all: bin/$(EXE_NAME)

# linking
bin/$(EXE_NAME): $(ALL_O)
	g++ $(OPTIONS) -o $@ $^ $(SFML_COMP)

# generic build instruction
build/%.o: src/%.cpp
	g++ $(OPTIONS) -c $< -o $@ -I include/ -MMD -MF $(subst build,dep,$(@:.o=.d))

clean:
	rm ./bin/* ./build/* ./dep/*

count:
	wc Makefile ./src/* ./include/*
CC = gcc
CPP = g++
CPPFLAGS = -std=c++20

ifeq ($(OS), Windows_NT)
	EXE = .exe
else
    EXE =
endif

.PHONY: build clean test

build: out/lemon$(EXE) out/render$(EXE) | out/lem

out:
	mkdir out

out/lem: | out
	mkdir out/lem

out/test: | out
	mkdir out/test

out/lemon$(EXE): src/lemon/lemon.c | out
	$(CC) src/lemon/lemon.c -o out/lemon$(EXE)

out/render$(EXE): src/render/*.cpp src/render/*.hpp | out
	$(CPP) $(CPPFLAGS) src/render/*.cpp -o out/render$(EXE)

out/test/render$(EXE): $(filter-out src/render/main.cpp,$(wildcard src/render/*.cpp)) src/render/*.hpp test/render/*.cpp test/testbed/*.cpp test/testbed/*.hpp | out/test
	$(CPP) $(CPPFLAGS) $(filter-out src/render/main.cpp,$(wildcard src/render/*.cpp)) test/render/*.cpp test/testbed/*.cpp -o out/test/render$(EXE)

test: out/test/render$(EXE)
	out/test/render$(EXE)

clean:
	rm -rf out

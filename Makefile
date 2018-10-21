CPP=g++
INCLUDES=#-I/usr/include/gtest
LIBS=-lSDL2
TEST_LIBS=-lgtest -lpthread
LDFLAGS=-L./lib $(LIBS)
CPPFLAGS=-c -g -Wall -Wpedantic -std=c++11 -pthread $(INCLUDES)

CLASSES = src/Emulator.cpp src/State.cpp src/Memory.cpp src/Timer.cpp src/Interrupt.cpp src/Display.cpp src/Input.cpp
MAIN_SRC = $(CLASSES) src/gbemu.cpp
TEST_SRC = $(CLASSES) src/tests/main.cpp src/tests/EmulatorTest.cpp src/tests/MemoryTest.cpp src/tests/DisplayTest.cpp src/tests/InputTest.cpp
MAIN_OBJ = $(MAIN_SRC:%.cpp=%.o)
TEST_OBJ = $(TEST_SRC:%.cpp=%.o)

MAIN_BIN = gbemu
TEST_BIN = test-gbemu

GTEST_SRC=/usr/src/gtest

.PHONY: all clean test

all: $(MAIN_BIN) $(TEST_BIN)

$(MAIN_BIN): $(MAIN_OBJ)
	$(CPP) $(MAIN_OBJ) $(LDFLAGS) -o $(MAIN_BIN)

$(TEST_BIN): $(TEST_OBJ) lib/libgtest.a
	$(CPP) $(TEST_OBJ) $(LDFLAGS) $(TEST_LIBS) -o $(TEST_BIN)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f $(MAIN_BIN) $(TEST_BIN) $(MAIN_OBJ) $(TEST_OBJ) lib/libgtest.a

test: $(TEST_BIN) $(MAIN_BIN)
	./$(TEST_BIN)
	./run_test_roms.sh

lib/libgtest.a:
	g++ -I$(GTEST_SRC) -pthread -fPIC -c $(GTEST_SRC)/src/gtest-all.cc -o lib/gtest-all.o
	ar -rv lib/libgtest.a lib/gtest-all.o
	rm lib/gtest-all.o

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M), arm64)
    CONFIG := config/config-macos.mk
else ifeq ($(UNAME_S), Linux)
    CONFIG := config/config-wsl-2025.mk
else
    $(error Unsupported platform: $(UNAME_S) $(UNAME_M). Add a config file in config/ and update this Makefile.)
endif

include $(CONFIG)

CC=g++
CCFLAGS=-g -c -std=c++17 -I$(ANTLRINC) -Iinclude -I. -Wno-attributes
LDFLAGS=-g

default: all
all: ifcc

##########################################
# link together all pieces of our compiler
OBJECTS=build/ifccBaseVisitor.o \
	build/ifccLexer.o \
	build/ifccVisitor.o \
	build/ifccParser.o \
	build/main.o \
	build/CodeGenVisitor.o

ifcc: $(OBJECTS)
	@mkdir -p build
	$(CC) $(LDFLAGS) build/*.o $(ANTLRLIB) -o build/ifcc

##########################################
# compile our hand-written C++ code: main(), CodeGenVisitor, etc.
build/%.o: src/%.cpp generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

##########################################
# compile all the antlr-generated C++
build/%.o: generated/%.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

# automagic dependency management: `gcc -MMD` generates all the .d files for us
-include build/*.d
build/%.d:

##########################################
# generate the C++ implementation of our Lexer/Parser/Visitor
generated/ifccLexer.cpp: generated/ifccParser.cpp
generated/ifccVisitor.cpp: generated/ifccParser.cpp
generated/ifccBaseVisitor.cpp: generated/ifccParser.cpp
generated/ifccParser.cpp: grammar/ifcc.g4
	@mkdir -p generated
	cd grammar && java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Cpp -o ../generated ifcc.g4

.PRECIOUS: generated/ifcc%.cpp

##########################################
# view the parse tree in a graphical window
# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= tests/cases/1_return42.c

gui:
	@mkdir -p generated build
	java -jar $(ANTLRJAR) -Dlanguage=Java -o generated grammar/ifcc.g4
	javac -cp $(ANTLRJAR) -d build generated/*.java
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# delete all machine-generated files
test: ifcc
	python3 tests/ifcc-test.py tests/cases

clean:
	rm -rf build generated ifcc-test-output

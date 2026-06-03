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
	build/SymbolTableVisitor.o \
	build/IRGeneratorVisitor.o \
	build/Block.o \
	build/ControlFlowGraph.o \
	build/backend/X86Backend.o \
	build/instructions/Add.o \
	build/instructions/Subtract.o \
	build/instructions/Multiply.o \
	build/instructions/Divide.o \
	build/instructions/Modulo.o \
	build/instructions/Copy.o \
	build/instructions/LoadConstant.o \
	build/instructions/Negate.o

ifcc: $(OBJECTS)
	@mkdir -p build
	@echo ">>> Linking build/ifcc..."
	@$(CC) $(LDFLAGS) $(OBJECTS) $(ANTLRLIB) -o build/ifcc
	@echo ">>> Build complete: build/ifcc"

##########################################
# compile our hand-written C++ code: main(), AsmGeneratorVisitor, etc.
build/%.o: src/%.cpp generated/ifccParser.cpp
	@mkdir -p $(dir $@)
	@echo "  Compiling $<..."
	@$(CC) $(CCFLAGS) -MMD -o $@ $<

##########################################
# compile all the antlr-generated C++
build/%.o: generated/%.cpp
	@mkdir -p build
	@echo "  Compiling [generated] $<..."
	@$(CC) $(CCFLAGS) -MMD -o $@ $<

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
	@echo ">>> Generating parser from grammar/ifcc.g4..."
	@cd grammar && java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Cpp -o ../generated ifcc.g4
	@echo "  Parser generated in generated/"

.PRECIOUS: generated/ifcc%.cpp

##########################################
# view the parse tree in a graphical window
# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= tests/cases/1_return42.c

gui:
	@mkdir -p generated build
	java -jar $(ANTLRJAR) -Dlanguage=Java -o generated grammar/ifcc.g4
	find generated -name "*.java" | xargs javac -cp $(ANTLRJAR) -d build
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# generate and view the assembly for a source file
# Usage: `make asm FILE=path/to/your/file.c`
asm: ifcc
	@mkdir -p build
	@./build/ifcc $(FILE) > build/$(notdir $(basename $(FILE))).s
	@echo ">>> Assembly written to build/$(notdir $(basename $(FILE))).s"
	@cat build/$(notdir $(basename $(FILE))).s

# generate GCC assembly via Docker (x86-64) for comparison with ifcc output
# Usage: `make docker-gcc-asm FILE=tests/cases/1_return42.c`
docker-gcc-asm: docker-build
	@$(DOCKER_RUN) sh -c "mkdir -p build && gcc -S -o build/$(notdir $(basename $(FILE))).gcc.s $(FILE)"
	@echo ">>> GCC assembly written to build/$(notdir $(basename $(FILE))).gcc.s"
	@cat build/$(notdir $(basename $(FILE))).gcc.s

##########################################
test: ifcc
	@echo ">>> Running tests..."
	@python3 tests/ifcc-test.py tests/cases

clean:
	@echo ">>> Cleaning build artifacts..."
	@rm -rf build generated ifcc-test-output
	@echo "  Done."

##########################################
# Docker : exécute make/make test/make clean dans le conteneur ifcc-dev.
# Le code est monté depuis l'hôte, seule la compilation tourne dans l'image.
DOCKER_IMAGE := ifcc-dev
DOCKER_RUN := docker run --rm -v "$(CURDIR)":/work -w /work $(DOCKER_IMAGE)

docker-build:
	@docker build -t $(DOCKER_IMAGE) .

docker: docker-build
	@$(DOCKER_RUN) make

docker-test: docker-build
	@$(DOCKER_RUN) make test | python3 tests/ifcc-pretty.py

docker-clean: docker-build
	@$(DOCKER_RUN) make clean

docker-asm: docker-build
	@$(DOCKER_RUN) make asm FILE=$(FILE)

docker-gui: docker-build
	@$(DOCKER_RUN) make gui FILE=$(FILE)

.PHONY: docker-build docker docker-test docker-clean docker-asm docker-gcc-asm

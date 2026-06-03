# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

```bash
make          # build the compiler → build/ifcc
make test     # build + run all tests in tests/cases/
make clean    # remove build/, generated/, ifcc-test-output/
make gui FILE=tests/cases/1_return42.c  # visualize parse tree (requires Java)
```

Run a single test manually:
```bash
./build/ifcc tests/cases/1_return42.c   # prints assembly to stdout
python3 tests/ifcc-test.py tests/cases/1_return42.c  # compare with GCC
```

## Platform config

The Makefile auto-detects the platform (`uname -s` / `uname -m`) and includes the matching file from `config/`:
- `config/config-macos.mk` — macOS Apple Silicon (arm64)
- `config/config-wsl-2025.mk` — Linux / WSL x86-64

To add a new platform, create a `config/config-<platform>.mk` with `ANTLRJAR`, `ANTLRINC`, and `ANTLRLIB` variables, then add a case in the Makefile.

## Architecture

The compiler pipeline is:

```
source .c  →  ANTLR lexer/parser  →  parse tree  →  SymbolTableVisitor  →  IRGeneratorVisitor  →  X86Backend  →  assembly stdout
```

**Grammar** (`grammar/ifcc.g4`) defines the language. ANTLR generates C++ lexer/parser/visitor stubs into `generated/` at build time. Never edit files in `generated/` — they are regenerated on every `make clean && make`.

**Three-pass pipeline**: `main.cpp` runs two visitors then a backend:
1. `SymbolTableVisitor` — first pass, builds a `map<string, VariableInfo>` (used flag) from variable declarations, checks semantics (declared? used?)
2. `IRGeneratorVisitor` — second pass, builds a `ControlFlowGraph` (CFG) of `Block`s containing `Instruction`s (Add, Subtract, Copy, LoadConstant, etc.)
3. `X86Backend` — walks the CFG and emits x86-64 AT&T assembly to `stdout` via `ControlFlowGraph::generateASM()`

All visitors extend the generated `ifccBaseVisitor`. Each `visitXxx` method corresponds to a grammar rule. To add language features: extend `ifcc.g4`, add `visitXxx` overrides in the visitors, add the `Instruction` subclass in `include/instructions/` + `src/instructions/`, and add the matching `emit()` overload in `Backend` and `X86Backend`.

**IR structure**: `ControlFlowGraph` owns a list of `Block`s; each `Block` owns a list of `Instruction`s. Variables are stored by name in the CFG's `variableMap` with positive offsets; `X86Backend::varToLocation` negates them to produce `-N(%rbp)` addresses. Temporary variables (`temp0`, `temp1`...) are allocated via `CFG::addTempVariable`. The special variable `$return` holds the function's return value and is loaded into `%eax` by the epilogue.

**Assembly output**: x86-64 AT&T syntax only. Division/modulo use `idivl` with `cdq` for sign-extension.

**Testing**: `tests/ifcc-test.py` compiles each `.c` in `tests/cases/` with both GCC and `build/ifcc`, runs both executables, and compares exit codes. A test passes when both compilers agree (both accept or both reject the program, and produce the same result).

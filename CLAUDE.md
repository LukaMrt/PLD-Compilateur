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
source .c  →  ANTLR lexer/parser  →  parse tree  →  CodeGenVisitor  →  assembly stdout
```

**Grammar** (`grammar/ifcc.g4`) defines the language. ANTLR generates C++ lexer/parser/visitor stubs into `generated/` at build time. Never edit files in `generated/` — they are regenerated on every `make clean && make`.

**Visitor pattern**: `CodeGenVisitor` (in `src/` and `include/`) extends the generated `ifccBaseVisitor`. Each `visitXxx` method corresponds to a grammar rule and emits assembly directly to `stdout`. To add language features: extend the grammar in `ifcc.g4`, then override the corresponding `visitXxx` method in `CodeGenVisitor`.

**Cross-platform assembly**: `CodeGenVisitor.cpp` uses `#ifdef __aarch64__` to emit ARM64 instructions (macOS Apple Silicon) vs x86-64 AT&T syntax (Linux). The entry point label also differs: `_main` on macOS, `main` on Linux (`#ifdef __APPLE__`).

**Testing**: `tests/ifcc-test.py` compiles each `.c` in `tests/cases/` with both GCC and `build/ifcc`, runs both executables, and compares exit codes. A test passes when both compilers agree (both accept or both reject the program, and produce the same result).

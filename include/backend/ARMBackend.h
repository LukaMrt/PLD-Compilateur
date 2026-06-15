#pragma once

#include "backend/Backend.h"

// Backend AArch64 (ARM64) ciblant macOS / Apple Silicon.
// Diffère de X86Backend sur trois points : architecture load/store (pas
// d'opérande mémoire-mémoire), registres w/x au lieu de %eax/%rax, et symboles
// préfixés par '_' (convention Mach-O de macOS).
class ARMBackend : public Backend
{
public:
    void emitBlockLabel(Block *block, std::ostream &output) override;
    void emitJump(Block *block, std::ostream &output) override;
    void emitFalseJump(Block *block, std::ostream &output) override;
    void emitPrologue(ControlFlowGraph *cfg, std::ostream &output) override;
    void emitEpilogue(ControlFlowGraph *cfg, std::ostream &output) override;

    std::string varToLocation(std::string name, ControlFlowGraph *cfg) override;
    std::string parameterToLocation(int index) override;

    void emit(Add *instr, std::ostream &output) override;
    void emit(Subtract *instr, std::ostream &output) override;
    void emit(Multiply *instr, std::ostream &output) override;
    void emit(Divide *instr, std::ostream &output) override;
    void emit(Modulo *instr, std::ostream &output) override;
    void emit(Copy *instr, std::ostream &output) override;
    void emit(LoadConstant *instr, std::ostream &output) override;
    void emit(Negate *instr, std::ostream &output) override;
    void emit(BitwiseAnd *instr, std::ostream &output) override;
    void emit(BitwiseOr *instr, std::ostream &output) override;
    void emit(BitwiseXor *instr, std::ostream &output) override;
    void emit(CallFunction *instr, std::ostream &output) override;
    void emit(Equal *instr, std::ostream &output) override;
    void emit(NotEqual *instr, std::ostream &output) override;
    void emit(Lesser *instr, std::ostream &output) override;
    void emit(Greater *instr, std::ostream &output) override;
    void emit(LesserOrEqual *instr, std::ostream &output) override;
    void emit(GreaterOrEqual *instr, std::ostream &output) override;
    void emit(Reference *instr, std::ostream &output) override;
    void emit(DereferenceRead *instr, std::ostream &output) override;
    void emit(DereferenceWrite *instr, std::ostream &output) override;
};

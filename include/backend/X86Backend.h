#pragma once

#include "backend/Backend.h"

class X86Backend : public Backend
{
public:
    void emitBlockLabel(Block *block, std::ostream &output) override;
    void emitJump(Block *block, std::ostream &output) override;
    void emitPrologue(ControlFlowGraph *cfg, std::ostream &output) override;
    void emitEpilogue(ControlFlowGraph *cfg, std::ostream &output) override;

    std::string varToLocation(std::string name, ControlFlowGraph *cfg) override;

    void emit(Add *instr, std::ostream &output) override;
    void emit(Subtract *instr, std::ostream &output) override;
    void emit(Multiply *instr, std::ostream &output) override;
    void emit(Divide *instr, std::ostream &output) override;
    void emit(Modulo *instr, std::ostream &output) override;
    void emit(Copy *instr, std::ostream &output) override;
    void emit(LoadConstant *instr, std::ostream &output) override;
    void emit(Negate *instr, std::ostream &output) override;
};

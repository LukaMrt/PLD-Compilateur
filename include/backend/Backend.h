#pragma once

#include <iostream>
#include <string>

class Block;
class ControlFlowGraph;
class Add;
class Subtract;
class Multiply;
class Divide;
class Modulo;
class Copy;
class LoadConstant;
class Negate;
class BitwiseAnd;
class BitwiseOr;
class BitwiseXor;
class CallFunction;
class Equal;
class NotEqual;
class Lesser;
class Greater;
class LesserOrEqual;
class GreaterOrEqual;
class DereferenceRead;
class DereferenceWrite;
class Reference;

class Backend
{
public:
    virtual ~Backend() = default;

    virtual void emitBlockLabel(Block *block, std::ostream &output) = 0;
    virtual void emitJump(Block *block, std::ostream &output) = 0;
    virtual void emitFalseJump(Block *block, std::ostream &output) = 0;
    virtual void emitPrologue(ControlFlowGraph *cfg, std::ostream &output) = 0;
    virtual void emitEpilogue(ControlFlowGraph *cfg, std::ostream &output) = 0;

    virtual std::string varToLocation(std::string name, ControlFlowGraph *cfg, int offset = 0) = 0;
    virtual std::string parameterToLocation(int index) = 0;

    virtual void emit(Add *instr, std::ostream &output) = 0;
    virtual void emit(Subtract *instr, std::ostream &output) = 0;
    virtual void emit(Multiply *instr, std::ostream &output) = 0;
    virtual void emit(Divide *instr, std::ostream &output) = 0;
    virtual void emit(Modulo *instr, std::ostream &output) = 0;
    virtual void emit(Copy *instr, std::ostream &output) = 0;
    virtual void emit(LoadConstant *instr, std::ostream &output) = 0;
    virtual void emit(Negate *instr, std::ostream &output) = 0;
    virtual void emit(BitwiseAnd *instr, std::ostream &output) = 0;
    virtual void emit(BitwiseOr *instr, std::ostream &output) = 0;
    virtual void emit(BitwiseXor *instr, std::ostream &output) = 0;
    virtual void emit(CallFunction *instr, std::ostream &output) = 0;
    virtual void emit(Equal *instr, std::ostream &output) = 0;
    virtual void emit(NotEqual *instr, std::ostream &output) = 0;
    virtual void emit(Lesser *instr, std::ostream &output) = 0;
    virtual void emit(Greater *instr, std::ostream &output) = 0;
    virtual void emit(LesserOrEqual *instr, std::ostream &output) = 0;
    virtual void emit(GreaterOrEqual *instr, std::ostream &output) = 0;
    virtual void emit(DereferenceRead *instr, std::ostream &output) = 0;
    virtual void emit(DereferenceWrite *instr, std::ostream &output) = 0;
    virtual void emit(Reference *instr, std::ostream &output) = 0;
};

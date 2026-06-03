#pragma once

#include <iostream>
#include <string>

class Instruction;
class Block;
class Function;

class Backend
{
public:
    virtual ~Backend() = default;

    virtual void emitInstruction(Instruction *instruction, std::ostream &output) = 0;
    virtual void emitBlockLabel(Block *block, std::ostream &output) = 0;
    virtual void emitJump(Block *block, std::ostream &output) = 0;
    virtual void emitPrologue(Function *function, std::ostream &output) = 0;
    virtual void emitEpilogue(Function *function, std::ostream &output) = 0;

    virtual std::string varToLocation(std::string name, Function *function) = 0;
};

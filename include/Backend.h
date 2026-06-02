#include <iostream>
#include <string>

using namespace std;

class Instruction;
class Block;
class Function;

class Backend
{
public:
    virtual ~Backend() = default;

    virtual void emitInstruction(Instruction *instruction, ostream &output) = 0;
    virtual void emitBlockLabel(Block *block, ostream &output) = 0;
    virtual void emitJump(Block *block, ostream &output) = 0;
    virtual void emitPrologue(Function *Function, ostream &output) = 0;
    virtual void emitEpilogue(Function *Function, ostream &output) = 0;

    // helper: converts an IR variable name to its target-specific location (e.g. "-4(%rbp)")
    virtual string varToLocation(string name, Function *Function) = 0;
};

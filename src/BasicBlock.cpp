#include "BasicBlock.h"

void BasicBlock::generateASM(Backend &backend, ostream &output)
{
    backend.emitBlockLabel(this, output);
    for (Instruction *instruction : instructions)
    {
        backend.emitInstruction(instruction, output);
    }
    if (true_case_block != nullptr && false_case_block != nullptr)
    {
        // this block ends with a conditional jump
        backend.emitJump(true_case_block, output);
    }
    else if (true_case_block != nullptr)
    {
        // this block ends with an unconditional jump to true_case_block
        backend.emitJump(true_case_block, output);
    }
    else if (false_case_block != nullptr)
    {
        // this block ends with an unconditional jump to false_case_block
        backend.emitJump(false_case_block, output);
    }
}
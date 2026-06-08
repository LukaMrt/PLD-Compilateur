#include "Block.h"

Block::Block(ControlFlowGraph *controlFlowGraph, std::string entry_label)
    : controlFlowGraph(controlFlowGraph), label(entry_label),
      true_case_block(nullptr), false_case_block(nullptr)
{
}

void Block::addInstruction(Instruction *instruction)
{
    instructions.push_back(instruction);
}

void Block::debug(std::ostream &output) const
{
    output << "[Block: " << label << "]\n";
    for (Instruction *instruction : instructions)
    {
        instruction->debug(output);
    }
}

void Block::generateASM(Backend &backend, std::ostream &output)
{
    backend.emitBlockLabel(this, output);
    for (Instruction *instruction : instructions)
    {
        instruction->generate(backend, output);
    }
    if (true_case_block != nullptr)
    {
        backend.emitJump(true_case_block, output);
    }
    else if (false_case_block != nullptr)
    {
        backend.emitFalseJump(false_case_block, output);
    }
}

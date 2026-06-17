#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class DereferenceRead : public Instruction
{
public:
    DereferenceRead(Block *block, Type type, std::string destination, std::string src, int offset = 0)
        : Instruction(block, type), destination(destination), src(src), offset(offset) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  DereferenceRead " << destination << " = *" << src << " + " << offset << "\n"; }

    std::string getDestination() const { return destination; }
    std::string getSrc() const { return src; }
    int getOffset() const { return offset; }

private:
    std::string destination;
    std::string src;
    int offset;
};

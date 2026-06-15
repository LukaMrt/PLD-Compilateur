#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class Reference : public Instruction
{
public:
    Reference(Block *block, Type type, std::string destination, std::string src)
        : Instruction(block, type), destination(destination), src(src) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  Reference     " << destination << " = &" << src << "\n"; }

    std::string getDestination() const { return destination; }
    std::string getSrc() const { return src; }

private:
    std::string destination;
    std::string src;
};

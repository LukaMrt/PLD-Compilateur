#pragma once

#include <string>
#include "../Instruction.h"
#include "../Backend.h"

class Copy : public Instruction
{
public:
    Copy(Block *block, Type type, std::string destination, std::string src)
        : Instruction(block, type), destination(destination), src(src) {}

    void generate(Backend &backend, std::ostream &output) override;

    std::string getDestination() const { return destination; }
    std::string getSrc() const { return src; }

private:
    std::string destination;
    std::string src;
};

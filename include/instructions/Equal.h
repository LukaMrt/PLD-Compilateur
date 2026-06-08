#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class Equal : public Instruction
{
public:
    Equal(Block *block, Type type, std::string destination, std::string left, std::string right)
        : Instruction(block, type), destination(destination), left(left), right(right) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  Equal    " << destination << " = " << left << " == " << right << "\n"; }

    std::string getDestination() const { return destination; }
    std::string getLeft() const { return left; }
    std::string getRight() const { return right; }

private:
    std::string destination;
    std::string left;
    std::string right;
};

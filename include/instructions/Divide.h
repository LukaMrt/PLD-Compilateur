#pragma once

#include <string>
#include "../Instruction.h"
#include "../Backend.h"

class Divide : public Instruction
{
public:
    Divide(Block *block, Type type, std::string destination, std::string left, std::string right)
        : Instruction(block, type), destination(destination), left(left), right(right) {}

    void generate(Backend &backend, std::ostream &output) override;

    std::string getDestination() const { return destination; }
    std::string getLeft() const { return left; }
    std::string getRight() const { return right; }

private:
    std::string destination;
    std::string left;
    std::string right;
};

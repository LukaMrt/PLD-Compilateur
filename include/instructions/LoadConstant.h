#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class LoadConstant : public Instruction
{
public:
    LoadConstant(Block *block, Type type, std::string destination, int value)
        : Instruction(block, type), destination(destination), value(value) {}

    void generate(Backend &backend, std::ostream &output) override;

    std::string getDestination() const { return destination; }
    int getValue() const { return value; }

private:
    std::string destination;
    int value;
};

#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class CallFunction : public Instruction
{
public:
    CallFunction(Block *block, Type type, std::string destination, std::string functionName)
        : Instruction(block, type), destination(destination), functionName(functionName) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override
    {
        output << "  Call          " << destination << " = " << functionName << "()\n";
    }

    std::string getDestination() const { return destination; }
    std::string getFunctionName() const { return functionName; }

private:
    std::string destination;
    std::string functionName;
};

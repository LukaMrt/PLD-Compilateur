#pragma once

#include <string>
#include <vector>
#include "../Instruction.h"
#include "backend/Backend.h"

class CallFunction : public Instruction
{
public:
    CallFunction(Block *block, Type type, std::string destination, std::string functionName, std::vector<std::string> arguments)
        : Instruction(block, type), destination(destination), functionName(functionName), arguments(arguments) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override
    {
        output << "  Call          " << destination << " = " << functionName << "()\n";
    }

    std::string getDestination() const { return destination; }
    std::string getFunctionName() const { return functionName; }
    std::vector<std::string> getArguments() const { return arguments; }
private:
    std::string destination;
    std::string functionName;
    std::vector<std::string> arguments;
};

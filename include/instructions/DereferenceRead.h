#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class DereferenceRead : public Instruction
{
public:
    DereferenceRead(Block *block, Type type, std::string destination, std::string src, int offset = 0, std::string index = "")
        : Instruction(block, type), destination(destination), src(src), offset(offset), index(index) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  DereferenceRead " << destination << " = *" << src << " + " << offset << (index.empty() ? "" : " [" + index + "]") << "\n"; }

    std::string getDestination() const { return destination; }
    std::string getSrc() const { return src; }
    int getOffset() const { return offset; }
    std::string getIndex() const { return index; }

private:
    std::string destination;
    std::string src;
    int offset;
    std::string index; // variable d'index runtime (vide = accès scalaire simple)
};

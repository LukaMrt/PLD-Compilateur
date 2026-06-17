#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class DereferenceWrite : public Instruction
{
public:
    DereferenceWrite(Block *block, Type type, std::string dest, std::string src, int offset = 0)
        : Instruction(block, type), dest(dest), src(src), offset(offset) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  DereferenceWrite *" << dest << " = " << src << " + " << offset << "\n"; }

    std::string getDest() const { return dest; }
    std::string getSrc() const { return src; }
    int getOffset() const { return offset; }

private:
    std::string dest; // la variable pointeur (p dans *p = ...)
    std::string src;  // la valeur à écrire
    int offset;
};

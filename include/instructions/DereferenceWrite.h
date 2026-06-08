#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class DereferenceWrite : public Instruction
{
public:
    DereferenceWrite(Block *block, Type type, std::string dest, std::string src)
        : Instruction(block, type), dest(dest), src(src) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  DereferenceWrite *" << dest << " = " << src << "\n"; }

    std::string getDest() const { return dest; }
    std::string getSrc() const { return src; }

private:
    std::string dest; // la variable pointeur (p dans *p = ...)
    std::string src;  // la valeur à écrire
};

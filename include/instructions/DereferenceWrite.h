#pragma once

#include <string>
#include "../Instruction.h"
#include "backend/Backend.h"

class DereferenceWrite : public Instruction
{
public:
    DereferenceWrite(Block *block, Type type, std::string dest, std::string src, int offset = 0, std::string index = "")
        : Instruction(block, type), dest(dest), src(src), offset(offset), index(index) {}

    void generate(Backend &backend, std::ostream &output) override;
    void debug(std::ostream &output) const override { output << "  DereferenceWrite *" << dest << " = " << src << " + " << offset << (index.empty() ? "" : " [" + index + "]") << "\n"; }

    std::string getDest() const { return dest; }
    std::string getSrc() const { return src; }
    int getOffset() const { return offset; }
    std::string getIndex() const { return index; }

private:
    std::string dest;   // la variable pointeur/zone (p dans *p = ..., a dans a[i] = ...)
    std::string src;    // la valeur à écrire
    int offset;         // offset statique
    std::string index;  // variable d'index runtime (vide = accès scalaire simple)
};

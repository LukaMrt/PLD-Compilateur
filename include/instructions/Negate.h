#include "../IR.h"
#include "../Backend.h"

class Negate : public Instruction
{
public:
    Negate(Block *block, Type type, string destination, string src)
        : Instruction(block, type), destination(destination), src(src) {}

    void generate(Backend &backend, ostream &output) override;

    string getDestination() const { return destination; }
    string getSrc() const { return src; }

private:
    string destination;
    string src;
};

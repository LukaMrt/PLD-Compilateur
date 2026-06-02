#include "../IR.h"
#include "../Backend.h"

class LoadConstant : public Instruction
{
public:
    LoadConstant(Block *block, Type type, string destination, int value)
        : Instruction(block, type), destination(destination), value(value) {}

    void generate(Backend &backend, ostream &output) override;

    string getDestination() const { return destination; }
    int getValue() const { return value; }

private:
    string destination;
    int value;
};

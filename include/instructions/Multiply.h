#include "../IR.h"
#include "../Backend.h"

class Multiply : public Instruction
{
public:
    Multiply(Block *block, Type type, string destination, string left, string right)
        : Instruction(block, type), destination(destination), left(left), right(right) {}

    void generate(Backend &backend, ostream &output) override;

    string getDestination() const { return destination; }
    string getLeft() const { return left; }
    string getRight() const { return right; }

private:
    string destination;
    string left;
    string right;
};

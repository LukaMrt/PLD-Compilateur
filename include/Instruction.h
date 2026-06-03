#include "Block.h"
#include "enum/Type.h"

class Instruction
{
public:
	Instruction(Block *block, Type type) : block(block), type(type) {}
	virtual ~Instruction() = default;

	virtual void generate(Backend &backend, ostream &output) = 0;

	Block *getBlock() const { return block; }
	Type getType() const { return type; }

protected:
	Type type;
	Block *block;
};
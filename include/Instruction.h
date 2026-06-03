#pragma once

#include <iostream>
#include "struct/Type.h"

class Block;
class Backend;

class Instruction
{
public:
	Instruction(Block *block, Type type) : block(block), type(type) {}
	virtual ~Instruction() = default;

	virtual void generate(Backend &backend, std::ostream &output) = 0;

	Block *getBlock() const { return block; }
	Type getType() const { return type; }

protected:
	Type type;
	Block *block;
};
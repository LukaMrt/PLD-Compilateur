#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Block.h"
#include "Backend.h"
#include "struct/Type.h"
#include "struct/Variable.h"

class ControlFlowGraph
{
public:
	ControlFlowGraph(std::string label);

	void generateASM(Backend &backend, std::ostream &output);

	// variable map management
	void addVariable(std::string name, Type type);
	std::string addTempVariable(Type type);
	Variable getVar(std::string name);
	std::string getOffset(std::string name);

	// blocks management
	void addBlock(Block *block);
	void setCurrentBlock(Block *block) { currentBlock = block; }
	Block *getCurrentBlock() { return currentBlock; }
	std::string getLabel() { return label; }

protected:
	Block *currentBlock;
	std::map<std::string, Variable> variableMap;
	std::vector<Block *> blocks;
};

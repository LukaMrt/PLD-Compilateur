#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Block.h"
#include "backend/Backend.h"
#include "utils/Type.h"
#include "utils/Variable.h"

class ControlFlowGraph
{
public:
	ControlFlowGraph(std::string label);

	void generateASM(Backend &backend, std::ostream &output);
	void debug(std::ostream &output) const;

	// variable map management
	void addVariable(std::string name, Type type, int pointerDepth = 0);
	std::string addTempVariable(Type type);
	Variable getVar(std::string name);
	std::string getOffset(std::string name);

	// parameters map management
	void addParameter(std::string name, Type type);
	std::vector<std::pair<std::string, Variable>> getParameters();
	std::string getParameterIndex(std::string name);

	// blocks management
	void addBlock(Block *block);
	void setCurrentBlock(Block *block) { currentBlock = block; }
	Block *getCurrentBlock() { return currentBlock; }
	Block *getExitBlock() { return exitBlock; }
	void setExitBlock(Block *block) { exitBlock = block; }
	std::string getLabel() { return label; }
	int getCurrentOffset() { return currentOffset; }

protected:
	std::string label;
	Block *currentBlock;
	Block *exitBlock;
	std::map<std::string, Variable> variableMap;
	std::map<std::string, Variable> parametersMap;
	std::vector<Block *> blocks;
	int currentOffset = 0;
};

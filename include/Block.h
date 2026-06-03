#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Instruction.h"
#include "Backend.h"

class ControlFlowGraph;

class Block
{
public:
	Block(ControlFlowGraph *controlFlowGraph, std::string entry_label);
	void generateASM(Backend &backend, std::ostream &output);

	void addInstruction(Instruction *instruction);

	std::string getLabel() { return label; }
	Block *getTrueCaseBlock() { return true_case_block; }
	Block *getFalseCaseBlock() { return false_case_block; }

	ControlFlowGraph *getControlFlowGraph() { return controlFlowGraph; }
	std::vector<Instruction *> getInstructions() { return instructions; }

private:
	ControlFlowGraph *controlFlowGraph;
	std::string label;
	Block *true_case_block;
	Block *false_case_block;
	std::string test_variable_name;
	std::vector<Instruction *> instructions;
};

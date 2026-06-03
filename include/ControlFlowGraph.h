#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "BasicBlock.h"
#include "Backend.h"
#include "enum/Type.h"

using namespace std;


class ControlFlowGraph
{
public:
	ControlFlowGraph(string label);

	string label;
	
	void generateASM(Backend &backend, ostream &output);
	
	// variable map management
	void addVariable(string name, Type type);
	string addTempVariable(Type type); // returns the name of the new variable
	Variable getVar(string name);
	string getOffset(string name);

	// blocks management
	void addBlock(BasicBlock *block);
	BasicBlock *currentBlock;

protected:
	map<string, Variable> variableMap;
	vector<BasicBlock *> blocks;
};
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Backend.h"
#include "struct/Type.h"
#include "ControlFlowGraph.h"

using namespace std;

string ControlFlowGraph::addTempVariable(Type type)
{
    string tempVarName = "temp" + to_string(variableMap.size());
    addVariable(tempVarName, type);
    return tempVarName;
}

void ControlFlowGraph::addVariable(string name, Type type)
{
    if (variableMap.find(name) != variableMap.end())
    {
        cerr << "Error: variable '" << name << "' already exists in the current scope." << endl;
        exit(1);
    }
    variableMap[name] = {type, (int)variableMap.size()};
}

void ControlFlowGraph::addBlock(BasicBlock *block)
{
    blocks.push_back(block);
    currentBlock = block;
}

void ControlFlowGraph::generateASM(Backend &backend, ostream &output)
{
    for (BasicBlock *block : blocks)
    {
        block->generateASM(backend, output);
    }
}


#include "ControlFlowGraph.h"

ControlFlowGraph::ControlFlowGraph(std::string label)
    : label(label), currentBlock(nullptr), currentOffset(0)
{
}

void ControlFlowGraph::addVariable(std::string name, Type type)
{
    if (variableMap.find(name) != variableMap.end())
    {
        std::cerr << "Error: variable '" << name << "' already exists in the current scope." << std::endl;
        exit(1);
    }
    currentOffset += typeSize(type);
    variableMap[name] = {type, currentOffset};
}

std::string ControlFlowGraph::addTempVariable(Type type)
{
    std::string tempVarName = "$temp" + std::to_string(variableMap.size());
    addVariable(tempVarName, type);
    return tempVarName;
}

Variable ControlFlowGraph::getVar(std::string name)
{
    if (variableMap.find(name) == variableMap.end())
    {
        std::cerr << "Error: variable '" << name << "' not found." << std::endl;
        exit(1);
    }
    return variableMap[name];
}

std::string ControlFlowGraph::getOffset(std::string name)
{
    return std::to_string(getVar(name).offset);
}

void ControlFlowGraph::addParameter(std::string name, Type type)
{
    int index = parametersMap.size();
    parametersMap[name] = {type, index};
}

std::vector<std::pair<std::string, Variable>> ControlFlowGraph::getParameters()
{
    return std::vector<std::pair<std::string, Variable>>(parametersMap.begin(), parametersMap.end());
}

std::string ControlFlowGraph::getParameterIndex(std::string name)
{
    if (parametersMap.find(name) == parametersMap.end())
    {
        std::cerr << "Error: parameter '" << name << "' not found." << std::endl;
        exit(1);
    }
    return std::to_string(parametersMap[name].offset);
}

void ControlFlowGraph::addBlock(Block *block)
{
    blocks.push_back(block);
    currentBlock = block;
}

void ControlFlowGraph::debug(std::ostream &output) const
{
    output << "=== CFG: " << label << " ===\n";
    output << "Parameters:\n";
    for (const auto &[name, var] : parametersMap)
    {
        output << "  [" << var.offset << "] " << name << "\n";
    }
    output << "Variables:\n";
    for (const auto &[name, var] : variableMap)
    {
        output << "  " << name << "  offset=" << var.offset << "\n";
    }
    output << "Blocks:\n";
    for (Block *block : blocks)
    {
        block->debug(output);
    }
    output << "\n";
}

void ControlFlowGraph::generateASM(Backend &backend, std::ostream &output)
{
    backend.emitPrologue(this, output);
    for (Block *block : blocks)
    {
        block->generateASM(backend, output);
    }
    backend.emitEpilogue(this, output);
}

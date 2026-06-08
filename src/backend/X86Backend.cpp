#include "backend/X86Backend.h"
#include "Block.h"
#include "ControlFlowGraph.h"
#include "instructions/Add.h"
#include "instructions/Subtract.h"
#include "instructions/Multiply.h"
#include "instructions/Divide.h"
#include "instructions/Modulo.h"
#include "instructions/Copy.h"
#include "instructions/LoadConstant.h"
#include "instructions/Negate.h"
#include "instructions/BitwiseAnd.h"
#include "instructions/BitwiseOr.h"
#include "instructions/BitwiseXor.h"
#include "instructions/CallFunction.h"
#include "instructions/Equal.h"
#include "instructions/NotEqual.h"
#include "instructions/Lesser.h"
#include "instructions/Greater.h"
#include "instructions/Lesser.h"
#include "instructions/Greater.h"
#include "instructions/LesserOrEqual.h"
#include "instructions/GreaterOrEqual.h"

void X86Backend::emitPrologue(ControlFlowGraph *cfg, std::ostream &output)
{
    output << ".globl " << cfg->getLabel() << "\n";
    output << cfg->getLabel() << ":\n";
    output << "    pushq %rbp\n";
    output << "    movq %rsp, %rbp\n";

    int paramIndex = 0;
    for (auto parameter : cfg->getParameters())
    {
        output << "    movl " << parameterToLocation(paramIndex) << ", " << varToLocation(parameter.first, cfg) << "\n";
        paramIndex++;
    }

    // Réserve le cadre de pile pour les variables locales, aligné sur 16 octets
    // (exigence de l'ABI System V) afin que %rsp soit en dessous des locales :
    // sinon un `call` écraserait celles-ci en y empilant l'adresse de retour.
    int frameSize = (cfg->getCurrentOffset() + 15) & ~15;
    if (frameSize > 0)
        output << "    subq $" << frameSize << ", %rsp\n";
}

void X86Backend::emitEpilogue(ControlFlowGraph *cfg, std::ostream &output)
{
    output << "    movl " << varToLocation("$return", cfg) << ", %eax\n";
    output << "    leave\n";
    output << "    ret\n";
}

void X86Backend::emitBlockLabel(Block *block, std::ostream &output)
{
    output << block->getLabel() << ":\n";
}

void X86Backend::emitJump(Block *block, std::ostream &output)
{
    output << "    jmp " << block->getLabel() << "\n";
}

std::string X86Backend::varToLocation(std::string name, ControlFlowGraph *cfg)
{
    return std::to_string(-cfg->getVar(name).offset) + "(%rbp)";
}

std::string X86Backend::parameterToLocation(int index)
{
    static const std::vector<std::string> paramRegs = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    if (index < paramRegs.size())
    {
        return paramRegs[index];
    }
    else
    {
        std::cerr << "Error: parameter index " << index << " out of range for x86-64 calling convention." << std::endl;
        exit(1);
    }
}

void X86Backend::emit(LoadConstant *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl $" << instr->getValue()
           << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Copy *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getSrc(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Negate *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getSrc(), cfg) << ", %eax\n";
    output << "    negl %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Add *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    addl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Subtract *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    subl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Multiply *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    imull " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Divide *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cdq\n";
    output << "    idivl " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Modulo *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cdq\n";
    output << "    idivl " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    movl %edx, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(BitwiseAnd *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    andl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(BitwiseOr *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    orl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(BitwiseXor *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    xorl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(CallFunction *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    auto args = instr->getArguments();
    for (size_t i = 0; i < args.size(); i++)
    {
        output << "    movl " << varToLocation(args[i], cfg) << ", " << parameterToLocation(i) << "\n";
    }
    output << "    call " << instr->getFunctionName() << "\n";
}

void X86Backend::emit(Equal *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    sete %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(NotEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    setne %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Lesser *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    setl %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Greater *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    setg %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(LesserOrEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    setle %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(GreaterOrEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    movl " << varToLocation(instr->getLeft(), cfg) << ", %eax\n";
    output << "    cmpl " << varToLocation(instr->getRight(), cfg) << ", %eax\n";
    output << "    setge %al\n";
    output << "    movzbl %al, %eax\n";
    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}
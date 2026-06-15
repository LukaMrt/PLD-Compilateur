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
#include "instructions/LesserOrEqual.h"
#include "instructions/GreaterOrEqual.h"
#include "instructions/Reference.h"
#include "instructions/DereferenceRead.h"
#include "instructions/DereferenceWrite.h"
#include <algorithm>

// Choix de la largeur d'instruction selon la taille de la valeur (en octets) :
// 8 octets (pointeur/double) → movq/%rax, sinon 4 octets → movl/%eax.
namespace
{
    const char *movOp(int size) { return size == 8 ? "movq" : "movl"; }
    const char *accReg(int size) { return size == 8 ? "%rax" : "%eax"; }
}

void X86Backend::emitPrologue(ControlFlowGraph *cfg, std::ostream &output)
{
    output << ".globl " << cfg->getLabel() << "\n";
    output << cfg->getLabel() << ":\n";
    output << "    pushq %rbp\n";
    output << "    movq %rsp, %rbp\n";

    int paramIndex = 0;
    for (auto parameter : cfg->getParameters())
    {
        if (paramIndex < 6)
        {
            output << "    movl " << parameterToLocation(paramIndex) << ", " << varToLocation(parameter.first, cfg) << "\n";
        }
        else
        {
            // Args 7+ : passés sur la pile par l'appelant, à des offsets positifs
            // depuis %rbp (16 = 7ᵉ arg, 24 = 8ᵉ, ...). On les recopie dans le cadre
            // local de la fonction pour les traiter comme des variables normales.
            int stackOffset = 16 + 8 * (paramIndex - 6);
            output << "    movl " << stackOffset << "(%rbp), %eax\n";
            output << "    movl %eax, " << varToLocation(parameter.first, cfg) << "\n";
        }
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

void X86Backend::emitFalseJump(Block *block, std::ostream &output)
{
    output << "    cmpl $0, %eax\n";
    output << "    je " << block->getLabel() << "\n";
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
    int size = cfg->getVar(instr->getDestination()).size();
    output << "    " << movOp(size) << " $" << instr->getValue()
           << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(Copy *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Si l'une des deux extrémités est un pointeur, on déplace 8 octets (movq)
    // pour ne pas tronquer l'adresse ; sinon 4 octets (movl) suffisent.
    int size = std::max(cfg->getVar(instr->getDestination()).size(), cfg->getVar(instr->getSrc()).size());
    output << "    " << movOp(size) << " " << varToLocation(instr->getSrc(), cfg) << ", " << accReg(size) << "\n";
    output << "    " << movOp(size) << " " << accReg(size) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
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

    size_t stackCount = args.size() > 6 ? args.size() - 6 : 0;
    bool needsPadding = (stackCount % 2) == 1;

    if (needsPadding)
        output << "    subq $8, %rsp\n";

    for (size_t i = args.size(); i-- > 6;)
    {
        output << "    movl " << varToLocation(args[i], cfg) << ", %eax\n";
        output << "    pushq %rax\n";
    }

    for (size_t i = 0; i < args.size() && i < 6; i++)
    {
        output << "    movl " << varToLocation(args[i], cfg) << ", " << parameterToLocation(i) << "\n";
    }

    output << "    call " << instr->getFunctionName() << "\n";

    int cleanup = 8 * stackCount + (needsPadding ? 8 : 0);
    if (cleanup > 0)
        output << "    addq $" << cleanup << ", %rsp\n";

    output << "    movl %eax, " << varToLocation(instr->getDestination(), cfg) << "\n";
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

void X86Backend::emit(Reference *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    leaq " << varToLocation(instr->getSrc(), cfg) << ", %rax\n";
    output << "    movq %rax, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(DereferenceRead *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Le pointeur lui-même se charge toujours sur 8 octets (movq). En revanche
    // la valeur lue dépend de la taille du résultat : déréférencer un T** rend
    // un pointeur (8 octets), déréférencer un T* rend un scalaire (4 octets).
    int size = cfg->getVar(instr->getDestination()).size();
    output << "    movq " << varToLocation(instr->getSrc(), cfg) << ", %rax\n";
    output << "    " << movOp(size) << " (%rax), " << accReg(size) << "\n";
    output << "    " << movOp(size) << " " << accReg(size) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void X86Backend::emit(DereferenceWrite *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // L'adresse de destination est toujours un pointeur (movq) ; la largeur de
    // la valeur écrite suit la taille de la source (pointeur ou scalaire).
    int size = cfg->getVar(instr->getSrc()).size();
    output << "    " << movOp(size) << " " << varToLocation(instr->getSrc(), cfg) << ", " << accReg(size) << "\n";
    output << "    movq " << varToLocation(instr->getDest(), cfg) << ", %rcx\n";
    output << "    " << movOp(size) << " " << accReg(size) << ", (%rcx)\n";
}

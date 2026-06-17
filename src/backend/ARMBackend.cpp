#include "backend/ARMBackend.h"
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

namespace
{
    // Sur AArch64, c'est le *nom* du registre qui encode la largeur : x0 = 64 bits
    // (pointeur), w0 = 32 bits (scalaire). Pas de suffixe d'instruction comme sur
    // x86 (movl/movq) ; ldr/str suffisent, leur taille suit le registre.
    const char *acc(int size) { return size == 8 ? "x0" : "w0"; }  // accumulateur
    const char *acc2(int size) { return size == 8 ? "x1" : "w1"; } // second opérande

    // Registres de passage d'argument AAPCS64 : x0–x7 (8 registres, contre 6 sur
    // x86). Au-delà, les arguments passent par la pile.
    const char *argReg(int index, int size)
    {
        static const char *regs64[] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};
        static const char *regs32[] = {"w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7"};
        return size == 8 ? regs64[index] : regs32[index];
    }

    // Mach-O (macOS) préfixe tous les symboles par un underscore : main → _main.
    // (Sur Linux/ELF on n'aurait pas ce préfixe.)
    std::string sym(const std::string &name) { return "_" + name; }
}

void ARMBackend::emitPrologue(ControlFlowGraph *cfg, std::ostream &output)
{
    output << ".globl " << sym(cfg->getLabel()) << "\n";
    output << sym(cfg->getLabel()) << ":\n";
    // stp = store pair : on empile en un coup x29 (frame pointer) et x30 (link
    // register, qui contient l'adresse de retour — l'équivalent de ce que `call`
    // empile implicitement sur x86). Le `!` post-décrémente sp de 16.
    output << "    stp x29, x30, [sp, #-16]!\n";
    output << "    mov x29, sp\n";

    // Cadre de pile aligné sur 16 octets (sp doit *toujours* l'être sur AArch64,
    // notamment à chaque `bl`). On l'alloue AVANT d'écrire les paramètres : ARM
    // n'a pas de "red zone", écrire sous sp non alloué serait incorrect.
    int frameSize = (cfg->getCurrentOffset() + 15) & ~15;
    if (frameSize > 0)
        output << "    sub sp, sp, #" << frameSize << "\n";

    int paramIndex = 0;
    for (auto parameter : cfg->getParameters())
    {
        int size = parameter.second.size();
        if (paramIndex < 8)
        {
            output << "    str " << argReg(paramIndex, size) << ", " << varToLocation(parameter.first, cfg) << "\n";
        }
        else
        {
            // Args 9+ : déposés par l'appelant au-dessus du cadre, à des offsets
            // positifs depuis x29 (16 = 9ᵉ arg, 24 = 10ᵉ, ...).
            int stackOffset = 16 + 8 * (paramIndex - 8);
            output << "    ldr " << acc(size) << ", [x29, #" << stackOffset << "]\n";
            output << "    str " << acc(size) << ", " << varToLocation(parameter.first, cfg) << "\n";
        }
        paramIndex++;
    }
}

void ARMBackend::emitEpilogue(ControlFlowGraph *cfg, std::ostream &output)
{
    // Valeur de retour dans w0 (équivalent de %eax).
    output << "    ldr w0, " << varToLocation("$return", cfg) << "\n";
    output << "    mov sp, x29\n";          // libère les locales (annule le sub)
    output << "    ldp x29, x30, [sp], #16\n"; // restaure fp/lr, dépile (post-inc)
    output << "    ret\n";
}

void ARMBackend::emitBlockLabel(Block *block, std::ostream &output)
{
    output << block->getLabel() << ":\n";
}

void ARMBackend::emitJump(Block *block, std::ostream &output)
{
    output << "    b " << block->getLabel() << "\n"; // 'b' = branch inconditionnel
}

void ARMBackend::emitFalseJump(Block *block, std::ostream &output)
{
    // On s'appuie, comme le backend x86 avec %eax, sur le fait que la dernière
    // instruction émise a laissé la valeur de la condition dans w0.
    output << "    cmp w0, #0\n";
    output << "    beq " << block->getLabel() << "\n";
}

std::string ARMBackend::varToLocation(std::string name, ControlFlowGraph *cfg, int offset)
{
    // Locales adressées relativement au frame pointer, sous celui-ci : [x29, #-N].
    // (Offsets limités à la portée de ldur/sub, suffisant pour de petits cadres.)
    return "[x29, #-" + std::to_string(cfg->getVar(name).offset + offset) + "]";
}

std::string ARMBackend::parameterToLocation(int index)
{
    static const std::vector<std::string> paramRegs = {"w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7"};
    if (index < paramRegs.size())
    {
        return paramRegs[index];
    }
    else
    {
        std::cerr << "Error: parameter index " << index << " out of range for AArch64 calling convention." << std::endl;
        exit(1);
    }
}

void ARMBackend::emit(LoadConstant *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    int size = cfg->getVar(instr->getDestination()).size();
    // Pas de "store immédiat" : on matérialise la constante dans un registre puis
    // on la range. Un immédiat 32 bits quelconque se charge en deux temps :
    // mov (16 bits bas) + movk (16 bits hauts, garde le reste). str pour finir.
    unsigned value = static_cast<unsigned>(instr->getValue());
    output << "    mov " << acc(size) << ", #" << (value & 0xffff) << "\n";
    output << "    movk " << acc(size) << ", #" << ((value >> 16) & 0xffff) << ", lsl #16\n";
    output << "    str " << acc(size) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Copy *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Si une extrémité est un pointeur, on transfère 8 octets (x0) pour ne pas
    // tronquer l'adresse ; sinon 4 (w0).
    int size = std::max(cfg->getVar(instr->getDestination()).size(), cfg->getVar(instr->getSrc()).size());
    output << "    ldr " << acc(size) << ", " << varToLocation(instr->getSrc(), cfg) << "\n";
    output << "    str " << acc(size) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Negate *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    ldr w0, " << varToLocation(instr->getSrc(), cfg) << "\n";
    output << "    neg w0, w0\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Add *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Schéma type des opérations binaires : charger les deux opérandes dans w0/w1,
    // calculer w0 = w0 op w1, ranger. ARM est à 3 opérandes (dest, src1, src2).
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    add w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Subtract *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    sub w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Multiply *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    mul w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Divide *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // sdiv (division signée) en une instruction : pas de cdq/idivl ni de paire
    // de registres edx:eax comme sur x86.
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    sdiv w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(Modulo *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Aucune instruction de modulo sur ARM : reste = a - (a / b) * b, calculé via
    // msub (multiply-subtract) : msub w0, w2, w1, w0  →  w0 = w0 - w2*w1.
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    sdiv w2, w0, w1\n";
    output << "    msub w0, w2, w1, w0\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(BitwiseAnd *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    and w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(BitwiseOr *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // orr (et non 'or'), eor (et non 'xor') : mnémoniques propres à ARM.
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    orr w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(BitwiseXor *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    output << "    ldr w0, " << varToLocation(instr->getLeft(), cfg) << "\n";
    output << "    ldr w1, " << varToLocation(instr->getRight(), cfg) << "\n";
    output << "    eor w0, w0, w1\n";
    output << "    str w0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(CallFunction *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    auto args = instr->getArguments();

    // Au-delà de 8 arguments, le surplus passe par la pile. On réserve un bloc
    // unique aligné sur 16 octets (sp doit rester aligné à l'appel) plutôt que
    // d'empiler un par un, ce qui casserait l'alignement.
    size_t stackCount = args.size() > 8 ? args.size() - 8 : 0;
    int stackSpace = (static_cast<int>(stackCount) * 8 + 15) & ~15;

    if (stackSpace > 0)
        output << "    sub sp, sp, #" << stackSpace << "\n";

    for (size_t k = 0; k < stackCount; k++)
    {
        size_t i = 8 + k;
        int size = cfg->getVar(args[i]).size();
        output << "    ldr " << acc(size) << ", " << varToLocation(args[i], cfg) << "\n";
        output << "    str " << acc(size) << ", [sp, #" << (k * 8) << "]\n";
    }

    // Les arguments dans les registres sont chargés depuis x29 (frame pointer),
    // donc insensibles au décalage de sp ci-dessus — un avantage de l'adressage
    // relatif au frame pointer plutôt qu'à sp.
    for (size_t i = 0; i < args.size() && i < 8; i++)
    {
        int size = cfg->getVar(args[i]).size();
        output << "    ldr " << argReg(i, size) << ", " << varToLocation(args[i], cfg) << "\n";
    }

    output << "    bl " << sym(instr->getFunctionName()) << "\n"; // bl = branch & link

    if (stackSpace > 0)
        output << "    add sp, sp, #" << stackSpace << "\n";

    int retSize = cfg->getVar(instr->getDestination()).size();
    output << "    str " << acc(retSize) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

namespace
{
    // Facteur commun des six comparaisons : ARM les ramène à un cmp suivi d'un
    // cset (conditional set) qui dépose 0/1 selon le drapeau. Une instruction
    // remplace le couple sete/movzbl du x86.
    void emitComparison(Backend *backend, const std::string &left, const std::string &right,
                        const std::string &dest, const char *cond,
                        ControlFlowGraph *cfg, std::ostream &output)
    {
        output << "    ldr w0, " << backend->varToLocation(left, cfg) << "\n";
        output << "    ldr w1, " << backend->varToLocation(right, cfg) << "\n";
        output << "    cmp w0, w1\n";
        output << "    cset w0, " << cond << "\n";
        output << "    str w0, " << backend->varToLocation(dest, cfg) << "\n";
    }
}

void ARMBackend::emit(Equal *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "eq", cfg, output);
}

void ARMBackend::emit(NotEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "ne", cfg, output);
}

void ARMBackend::emit(Lesser *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "lt", cfg, output);
}

void ARMBackend::emit(Greater *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "gt", cfg, output);
}

void ARMBackend::emit(LesserOrEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "le", cfg, output);
}

void ARMBackend::emit(GreaterOrEqual *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    emitComparison(this, instr->getLeft(), instr->getRight(), instr->getDestination(), "ge", cfg, output);
}

void ARMBackend::emit(Reference *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Équivalent du leaq : on calcule l'adresse de la locale au lieu de la lire.
    // Pas de leaq sur ARM ; l'adresse = x29 - offset, via une soustraction.
    int offset = cfg->getVar(instr->getSrc()).offset;
    output << "    sub x0, x29, #" << offset << "\n";
    output << "    str x0, " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(DereferenceRead *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // Le pointeur se charge toujours sur 8 octets (x0). La valeur lue suit la
    // taille du résultat : *(T**) rend un pointeur (8), *(T*) un scalaire (4).
    int size = cfg->getVar(instr->getDestination()).size();
    output << "    ldr x0, " << varToLocation(instr->getSrc(), cfg) << "\n";
    output << "    ldr " << acc2(size) << ", [x0]\n";
    output << "    str " << acc2(size) << ", " << varToLocation(instr->getDestination(), cfg) << "\n";
}

void ARMBackend::emit(DereferenceWrite *instr, std::ostream &output)
{
    ControlFlowGraph *cfg = instr->getBlock()->getControlFlowGraph();
    // L'adresse de destination est toujours un pointeur (x1) ; la largeur écrite
    // suit la taille de la source.
    int size = cfg->getVar(instr->getSrc()).size();
    output << "    ldr " << acc(size) << ", " << varToLocation(instr->getSrc(), cfg) << "\n";
    output << "    ldr x1, " << varToLocation(instr->getDest(), cfg) << "\n";
    output << "    str " << acc(size) << ", [x1]\n";
}

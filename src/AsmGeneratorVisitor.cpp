#include "AsmGeneratorVisitor.h"

antlrcpp::Any AsmGeneratorVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    std::cout << ".globl main\n";
    std::cout << " main: \n";

    // Prologue
    std::cout << "    # Prologue\n";
    std::cout << "    push %rbp         # save %rbp on the stack\n";
    std::cout << "    mov %rsp, %rbp    # define %rbp for the current function\n";

    // Reserve stack space for local variables (aligned on 16 bytes)
    // so that push/pop used as scratch storage don't clobber them.
    int stackSize = symbolTable.size() * 4;
    int alignedStackSize = (stackSize + 15) / 16 * 16; // round up to next multiple of 16 (ABI requirement)
    if (alignedStackSize > 0)
    {
        std::cout << "    subq $" << alignedStackSize << ", %rsp   # allocate room for local variables\n";
    }
    std::cout << "\n";

    this->visit(ctx->bloc());
    /*for (auto statement : ctx->statement())
    {
        this->visit(statement);
    }*/

    // Epilogue
    std::cout << "\n";
    std::cout << "    # Epilogue\n";
    std::cout << "    mov %rbp, %rsp    # discard local variables / restore stack pointer\n";
    std::cout << "    pop %rbp         # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitVariable_creation(ifccParser::Variable_creationContext *ctx)
{
    return visitChildren(ctx);
}


antlrcpp::Any AsmGeneratorVisitor::visitVariable_creation_with_initialization(ifccParser::Variable_creation_with_initializationContext *ctx)
{
    std::string varName = ctx->VARIABLE()->getText();
    int offset = symbolTable.at(varName).offset;

    this->visit(ctx->instruction());
    std::cout << "    movl %eax, " << offset << "(%rbp)\n";

    return 0;
}


antlrcpp::Any AsmGeneratorVisitor::visitVariable_creation_without_initialization(ifccParser::Variable_creation_without_initializationContext *ctx)
{
    std::string varName = ctx->VARIABLE()->getText();
    int offset = symbolTable.at(varName).offset;

    std::cout << "    movl $0, " << offset << "(%rbp)\n";

    return 0;
}   

// DELETED
/*antlrcpp::Any AsmGeneratorVisitor::visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx)
{
    if (ctx->variable_creation())
    {
        return this->visit(ctx->variable_creation());
    }

    std::string varName = ctx->VARIABLE()->getText();
    int offset = symbolTable.at(varName).offset;

    this->visit(ctx->expression());
    std::cout << "    movl %eax, " << offset << "(%rbp)\n";

    return 0;
}*/

antlrcpp::Any AsmGeneratorVisitor::visitReturn_statement(ifccParser::Return_statementContext *ctx)
{
    this->visit(ctx->expression());
    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitUnary_operation(ifccParser::Unary_operationContext *ctx)
{
    this->visit(ctx->expression());
    
    if (ctx->op->getType() == ifccParser::MINUS)
    {
        std::cout << "    neg %eax\n";                    // unary minus: negate
    }
    else if (ctx->op->getType() == ifccParser::NOT)
    {
        // IA generated
        std::cout << "    test %eax, %eax\n";             // set ZF if %eax == 0
        std::cout << "    sete %al\n";                     // set %al to 1 if ZF is set, 0 otherwise
        std::cout << "    movzbl %al, %eax\n";            // zero-extend %al to %eax
    }
    
    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitAdditive_expression(ifccParser::Additive_expressionContext *ctx)
{
    // Evaluate left operand, save it on the stack, then evaluate right operand.
    // After this, %eax = right operand and %ebx = left operand.
    this->visit(ctx->expression(0));
    std::cout << "    push %rax\n";
    this->visit(ctx->expression(1));
    std::cout << "    pop %rbx\n";

    if (ctx->op->getType() == ifccParser::PLUS)
    {
        std::cout << "    add %ebx, %eax\n";        // %eax = left + right
    }
    else
    {
        std::cout << "    sub %eax, %ebx\n"; // %ebx = left - right
        std::cout << "    mov %ebx, %eax\n";
    }

    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx)
{
    // Same operand-evaluation scheme: %eax = right operand, %ebx = left operand.
    this->visit(ctx->expression(0));
    std::cout << "    push %rax\n";
    this->visit(ctx->expression(1));
    std::cout << "    pop %rbx\n";

    if (ctx->op->getType() == ifccParser::TIMES)
    {
        std::cout << "    imul %ebx, %eax\n";       // %eax = left * right
        return 0;
    }

    // Division and modulo both use idivl: dividend (left) in %eax, divisor (right) in %ebx.
    std::cout << "    xchg %eax, %ebx\n";           // %eax = left (dividend), %ebx = right (divisor)
    std::cout << "    cdq\n";                       // sign-extend %eax into %edx:%eax
    std::cout << "    idivl %ebx\n";                // quotient in %eax, remainder in %edx

    if (ctx->op->getType() == ifccParser::MODULO)
    {
        std::cout << "    movl %edx, %eax\n";       // modulo: keep the remainder
    }

    return 0;
}

// TODO vraiment besoin de surcharger ? C'est le comportement de base
antlrcpp::Any AsmGeneratorVisitor::visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx)
{
    return this->visit(ctx->instruction());
}

antlrcpp::Any AsmGeneratorVisitor::visitConstant_expression(ifccParser::Constant_expressionContext *ctx)
{
    int value = std::stoi(ctx->CONSTANT()->getText());
    std::cout << "    movl $" << value << ", %eax\n";
    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    int offset = symbolTable.at(ctx->VARIABLE()->getText()).offset;
    std::cout << "    movl " << offset << "(%rbp), %eax\n";
    return 0;
}

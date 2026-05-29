#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    std::cout << ".globl main\n";
    std::cout << " main: \n";

    // Prologue
    std::cout << "    # Prologue\n";
    std::cout << "    push %rbp         # save %rbp on the stack\n";
    std::cout << "    mov %rsp, %rbp    # define %rbp for the current function\n";
    std::cout << "\n";

    for (auto statement : ctx->statement())
    {
        this->visit(statement);
    }

    // Epilogue
    std::cout << "\n";
    std::cout << "    # Epilogue\n";
    std::cout << "    pop %rbp         # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitVariable_creation(ifccParser::Variable_creationContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    int offset = symbolTable.at(varName).offset;

    auto expr = ctx->expression();

    if (!expr)
    {
        std::cout << "    movl $0, " << offset << "(%rbp)  # initialize variable " << varName << " with default value 0\n";
        return 0;
    }

    if (expr && expr->CONST())
    {
        int value = stoi(expr->CONST()->getText());
        std::cout << "    movl $" << value << ", " << offset << "(%rbp)  # initialize variable " << varName << " with value " << value << "\n";
    }

    if (expr && expr->VAR())
    {
        std::string targetVarName = expr->VAR()->getText();
        int targetOffset = symbolTable.at(targetVarName).offset;
        std::cout << "    movl " << targetOffset << "(%rbp), %eax  # load value of variable " << targetVarName << " into %eax (1/2)\n";
        std::cout << "    movl %eax, " << offset << "(%rbp)  # initialize variable " << varName << " with value of variable " << targetVarName << " (2/2)\n";
    }


    return 0;
}

antlrcpp::Any CodeGenVisitor::visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx)
{
    if (ctx->variable_creation())
    {
        return this->visit(ctx->variable_creation());
    }

    std::string varName = ctx->VAR()->getText();
    int offset = symbolTable.at(varName).offset;

    auto expr = ctx->expression();

    if (expr->CONST())
    {
        int value = stoi(expr->CONST()->getText());
        std::cout << "    movl $" << value << ", " << offset << "(%rbp)  # assign value " << value << " to variable " << varName << "\n";
        return 0;
    }

    if (expr->VAR())
    {
        std::string targetVarName = expr->VAR()->getText();
        int targetOffset = symbolTable.at(targetVarName).offset;
        std::cout << "    movl " << targetOffset << "(%rbp), %eax  # load value of variable " << targetVarName << " into %eax (1/2)\n";
        std::cout << "    movl %eax, " << offset << "(%rbp)  # assign value of variable " << targetVarName << " to variable " << varName << " (2/2)\n";
        return 0;
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_statement(ifccParser::Return_statementContext *ctx)
{
    auto expr = ctx->expression();

    if (expr->CONST())
    {
        int retval = stoi(expr->CONST()->getText());
        std::cout << "    movl $" << retval << ", %eax  # load return value " << retval << " into %eax\n";
    }
    else if (expr->VAR())
    {
        std::string varName = expr->VAR()->getText();
        int offset = symbolTable.at(varName).offset;
        std::cout << "    movl " << offset << "(%rbp), %eax  # load return value from variable " << varName << " into %eax\n";
    }

    return 0;
}

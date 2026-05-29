#include "AsmGeneratorVisitor.h"

antlrcpp::Any AsmGeneratorVisitor::visitProg(ifccParser::ProgContext *ctx)
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

antlrcpp::Any AsmGeneratorVisitor::visitVariable_creation(ifccParser::Variable_creationContext *ctx)
{
    std::string varName = ctx->VAR()->getText();
    int offset = symbolTable.at(varName).offset;

    auto expr = ctx->expression();

    if (!expr)
    {
        std::cout << "    movl $0, " << offset << "(%rbp)  # initialize variable " << varName << " with default value 0\n";
        return 0;
    }

    this->visit(expr);
    std::cout << "    movl %eax, " << offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx)
{
    if (ctx->variable_creation())
    {
        return this->visit(ctx->variable_creation());
    }

    std::string varName = ctx->VAR()->getText();
    int offset = symbolTable.at(varName).offset;

    this->visit(ctx->expression());
    std::cout << "    movl %eax, " << offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitReturn_statement(ifccParser::Return_statementContext *ctx)
{
    this->visit(ctx->expression());
    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitConst_expression(ifccParser::Const_expressionContext *ctx)
{
    int value = std::stoi(ctx->CONST()->getText());
    std::cout << "    movl $" << value << ", %eax\n";
    return 0;
}

antlrcpp::Any AsmGeneratorVisitor::visitVar_expression(ifccParser::Var_expressionContext *ctx)
{
    int offset = symbolTable.at(ctx->VAR()->getText()).offset;
    std::cout << "    movl " << offset << "(%rbp), %eax\n";
    return 0;
}

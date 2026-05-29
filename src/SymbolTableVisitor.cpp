#include "SymbolTableVisitor.h"

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    for (auto statement : ctx->statement())
    {
        this->visit(statement);
    }

    for (auto &entry : symbolTable)
    {
        if (!entry.second.used)
        {
            std::cerr << "Warning: variable '" << entry.first << "' declared but never used." << std::endl;
        }
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx)
{
    if (ctx->variable_creation())
    {
        return this->visit(ctx->variable_creation());
    }

    std::string varName = ctx->VAR()->getText();
    if (!isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' not declared." << std::endl;
        exit(1);
    }

    if (ctx->expression())
    {
        auto expr = ctx->expression();
        if (expr->VAR())
        {
            std::string targetVarName = expr->VAR()->getText();
            if (!isDeclared(targetVarName))
            {
                std::cerr << "Error: variable '" << targetVarName << "' not declared." << std::endl;
                exit(1);
            }
        }
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_creation(ifccParser::Variable_creationContext *ctx)
{
    std::string varName = ctx->VAR()->getText();

    if (isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' already declared." << std::endl;
        exit(1);
    }

    symbolTable[varName] = {nextOffset, false};
    nextOffset -= 4;

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpression(ifccParser::ExpressionContext *ctx)
{
    if (ctx->VAR())
    {
        std::string varName = ctx->VAR()->getText();

        if (!isDeclared(varName))
        {
            std::cerr << "Error: variable '" << varName << "' not declared." << std::endl;
            exit(1);
        }

        symbolTable[varName].used = true;
        return symbolTable[varName].offset;
    }

    return 0;
}

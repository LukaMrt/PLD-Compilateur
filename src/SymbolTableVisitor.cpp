#include "SymbolTableVisitor.h"

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    this->visit(ctx->bloc());
    // for (auto statement : ctx->bloc())
    // {
    //     this->visit(statement);
    // }

    for (auto &entry : symbolTable)
    {
        if (!entry.second.used)
        {
            std::cerr << "Warning: variable '" << entry.first << "' declared but never used." << std::endl;
        }
    }

    return 0;
}

/*antlrcpp::Any SymbolTableVisitor::visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx)
{
    if (ctx->variable_creation())
    {
        return this->visit(ctx->variable_creation());
    }

    std::string varName = ctx->VARIABLE()->getText();
    if (!isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' not declared." << std::endl;
        exit(1);
    }

    this->visit(ctx->expression());

    return 0;
}*/

antlrcpp::Any SymbolTableVisitor::visitVariable_creation(ifccParser::Variable_creationContext *ctx)
{
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitVariable_creation_without_initialization(ifccParser::Variable_creation_without_initializationContext *ctx)
{
    std::string varName = ctx->VARIABLE()->getText();

    if (isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' is already declared." << std::endl;
        exit(1);
    }
    symbolTable[varName] = {nextOffset, false};
    nextOffset -= 4;
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_creation_with_initialization(ifccParser::Variable_creation_with_initializationContext *ctx)
{
    std::string varName = ctx->VARIABLE()->getText();

    if (isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' is already declared." << std::endl;
        exit(1);
    }
    symbolTable[varName] = {nextOffset, false};
    nextOffset -= 4;
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    std::string varName = ctx->VARIABLE()->getText();

    if (!isDeclared(varName))
    {
        std::cerr << "Error: variable '" << varName << "' not declared." << std::endl;
        exit(1);
    }

    symbolTable[varName].used = true;
    return symbolTable[varName].offset;
}

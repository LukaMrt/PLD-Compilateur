#include "SymbolTableVisitor.h"

antlrcpp::Any SymbolTableVisitor::visitFunction(ifccParser::FunctionContext *ctx)
{
    this->visitChildren(ctx);

    for (auto &entry : symbolTable)
    {
        if (!entry.second.used)
        {
            std::cerr << "Warning: variable '" << entry.first << "' defined but never used." << std::endl;
        }
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitFunction_variable_declaration(ifccParser::Function_variable_declarationContext *ctx)
{
    for (auto variableName : ctx->IDENTIFIER())
    {
        std::string varName = variableName->getText();
        this->declareVariable(varName);
    }
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->declareVariable(varName);
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->declareVariable(varName);
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
    if (ctx->IDENTIFIER() != nullptr)
    {
        std::string varName = ctx->IDENTIFIER()->getText();
        this->useVariable(varName);
    }
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->useVariable(varName);
    return 0;
}

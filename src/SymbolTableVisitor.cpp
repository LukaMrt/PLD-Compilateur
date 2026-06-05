#include "SymbolTableVisitor.h"

antlrcpp::Any SymbolTableVisitor::visitFunction(ifccParser::FunctionContext *ctx)
{
    currentFunction = ctx->IDENTIFIER()->getText();
    allSymbolTables[currentFunction] = {};

    this->visitChildren(ctx);

    for (auto &entry : currentTable())
    {
        if (!entry.second.used)
        {
            std::cerr << "Warning: variable '" << entry.first << "' defined but never used." << std::endl;
        }
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = stringToType(ctx->TYPE()->getText());
    this->declareVariable(varName, type);
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    // Le TYPE n'est pas sur le variable_definition mais sur son parent
    // variable_declaration ('int' partagé par 'int a, b, c;').
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);
    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()));
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);
    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
    for (auto &id : ctx->IDENTIFIER())
    {
        this->checkDeclared(id->getText());
    }
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->useVariable(varName);
    return 0;
}

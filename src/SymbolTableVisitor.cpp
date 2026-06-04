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
    // Chaque paramètre porte son propre TYPE, apparié par index avec son IDENTIFIER.
    for (size_t i = 0; i < ctx->IDENTIFIER().size(); ++i)
    {
        std::string varName = ctx->IDENTIFIER(i)->getText();
        Type type = stringToType(ctx->TYPE(i)->getText());
        this->declareVariable(varName, type);
    }
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

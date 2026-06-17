#include "SymbolTableVisitor.h"

#include <set>

// Fonctions de la libc tolérées sans définition dans le fichier source.
static const std::set<std::string> libcFunctions = {
    "putchar",
    "getchar",
};

antlrcpp::Any SymbolTableVisitor::visitFunction(ifccParser::FunctionContext *ctx)
{
    currentFunction = ctx->IDENTIFIER()->getText();
    if (functionTable.find(currentFunction) != functionTable.end())
    {
        std::cerr << "Error: function '" << currentFunction << "' is already defined." << std::endl;
        exit(1);
    }
    allSymbolTables[currentFunction] = {};
    functionTable[currentFunction] = {stringToType(ctx->TYPE()->getText()), {}};

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
    int pointerDepth = ctx->TIMES().size();

    this->declareVariable(varName, type, pointerDepth);
    functionTable[currentFunction].parameterTypes.push_back(type);
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->left_value()->IDENTIFIER()->getText();
    // Le TYPE n'est pas sur le variable_definition mais sur son parent
    // variable_declaration ('int' partagé par 'int a, b, c;').
    int pointerDepth = ctx->left_value()->TIMES().size();
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);

    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()), pointerDepth);
    return visit(ctx->expression());
}

antlrcpp::Any SymbolTableVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->left_value()->IDENTIFIER()->getText();
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);
    int pointerDepth = ctx->left_value()->TIMES().size();
    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()), pointerDepth);
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitLeft_value(ifccParser::Variable_left_valueContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->checkDeclared(varName);
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitTable_definition(ifccParser::Table_definitionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);
    int pointerDepth = 1;
    int size = ctx->CONSTANT() ? std::stoi(ctx->CONSTANT()->getText()) : -1;
    int size_table_init = ctx->table_init() ? ctx->table_init()->expression().size() : -1;
    if (size_table_init != -1 && size != -1 && size_table_init != size)
    {
        std::cerr << "Error: table '" << varName << "' has size " << size
                  << " but is initialized with " << size_table_init << " values." << std::endl;
        exit(1);
    } 
    int final_size = (size != -1) ? size : size_table_init;
    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()), pointerDepth);
    return visitChildren(ctx);
}


antlrcpp::Any SymbolTableVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    this->useVariable(varName);
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitFunction_call(ifccParser::Function_callContext *ctx)
{
    std::string funcName = ctx->IDENTIFIER()->getText();
    auto it = functionTable.find(funcName);
    if (it == functionTable.end() && libcFunctions.find(funcName) == libcFunctions.end())
    {
        std::cerr << "Error: function '" << funcName << "' is not defined." << std::endl;
        exit(1);
    }

    // L'arité n'est vérifiée que pour les fonctions définies localement
    // (la signature des fonctions libc n'est pas connue).
    if (it != functionTable.end())
    {
        size_t expected = it->second.parameterTypes.size();
        size_t actual = ctx->expression().size();
        if (expected != actual)
        {
            std::cerr << "Error: function '" << funcName << "' expects " << expected
                      << " argument(s) but " << actual << " were given." << std::endl;
            exit(1);
        }
    }

    return visitChildren(ctx);
}

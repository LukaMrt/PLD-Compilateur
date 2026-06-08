#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"
#include "ControlFlowGraph.h"

#include <map>
#include <string>

class IRGeneratorVisitor : public ifccBaseVisitor
{
public:
    IRGeneratorVisitor(const std::map<std::string, std::map<std::string, SymbolTableVisitor::VariableInfo>> &allSymbolTables)
        : allSymbolTables(allSymbolTables), currentCFG(nullptr) {}

    ControlFlowGraph *getCurrentCFG() const { return currentCFG; }

    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
    virtual antlrcpp::Any visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(ifccParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
    virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
    virtual antlrcpp::Any visitConstant_expression(ifccParser::Constant_expressionContext *ctx) override;
    virtual antlrcpp::Any visitCharacter_expression(ifccParser::Character_expressionContext *ctx) override;
    virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
    virtual antlrcpp::Any visitUnary_operation(ifccParser::Unary_operationContext *ctx) override;
    virtual antlrcpp::Any visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx) override;
    virtual antlrcpp::Any visitAdditive_expression(ifccParser::Additive_expressionContext *ctx) override;
    virtual antlrcpp::Any visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_and_expression(ifccParser::Bitwise_and_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_or_expression(ifccParser::Bitwise_or_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_xor_expression(ifccParser::Bitwise_xor_expressionContext *ctx) override;
    virtual antlrcpp::Any visitEqual_expression(ifccParser::Equal_expressionContext *ctx) override;
    virtual antlrcpp::Any visitComparison_expression(ifccParser::Comparison_expressionContext *ctx) override;

private:
    // Optimisation par pliage de constantes : associe un nom de variable temporaire
    // à sa valeur connue à la compilation. Ne contient que des temps non mutables.
    std::map<std::string, int> knownConstants;

    // Renvoie true si v est une constante connue ; place alors sa valeur dans out.
    bool isConstant(const std::string &v, int &out) const;
    // Émet un LoadConstant dans un nouveau temp, l'enregistre comme constant et renvoie son nom.
    std::string emitConstant(Type type, int value);

    std::map<std::string, std::map<std::string, SymbolTableVisitor::VariableInfo>> allSymbolTables;
    std::map<std::string, SymbolTableVisitor::VariableInfo> symbolTable;
    std::map<std::string, ControlFlowGraph *> cfgs;
    // Type de retour de chaque fonction, renseigné lors de la visite de sa définition.
    std::map<std::string, Type> functionReturnTypes;
    ControlFlowGraph *currentCFG;
    int deadBlockCount = 0;

public:
    const std::map<std::string, ControlFlowGraph *> &getCFGs() const { return cfgs; }
    int getDeadBlockCount() const { return deadBlockCount; }
};

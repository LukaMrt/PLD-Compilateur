#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"

#include <map>
#include <string>

class AsmGeneratorVisitor : public ifccBaseVisitor {
    public:
        AsmGeneratorVisitor(const std::map<std::string, SymbolTableVisitor::VariableInfo> &symbolTable)
            : symbolTable(symbolTable) {}

        virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
        virtual antlrcpp::Any visitReturn_statement(ifccParser::Return_statementContext *ctx) override;
        virtual antlrcpp::Any visitVariable_declaration(ifccParser::Variable_declarationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
        virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
        virtual antlrcpp::Any visitConstant_expression(ifccParser::Constant_expressionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
        virtual antlrcpp::Any visitUnary_operation(ifccParser::Unary_operationContext *ctx) override;
        virtual antlrcpp::Any visitAdditive_expression(ifccParser::Additive_expressionContext *ctx) override;
        virtual antlrcpp::Any visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx) override;

    private:
        std::map<std::string, SymbolTableVisitor::VariableInfo> symbolTable;
};


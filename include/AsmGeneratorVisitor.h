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

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_statement(ifccParser::Return_statementContext *ctx) override;
        virtual antlrcpp::Any visitVariable_creation(ifccParser::Variable_creationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_creation_with_initialization(ifccParser::Variable_creation_with_initializationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_creation_without_initialization(ifccParser::Variable_creation_without_initializationContext *ctx) override;
        // virtual antlrcpp::Any visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx) override;
        virtual antlrcpp::Any visitConstant_expression(ifccParser::Constant_expressionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
        virtual antlrcpp::Any visitUnary_operation(ifccParser::Unary_operationContext *ctx) override;
        virtual antlrcpp::Any visitAdditive_expression(ifccParser::Additive_expressionContext *ctx) override;
        virtual antlrcpp::Any visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx) override;
        virtual antlrcpp::Any visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx) override;

    private:
        std::map<std::string, SymbolTableVisitor::VariableInfo> symbolTable;
};


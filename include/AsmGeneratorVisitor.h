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
        virtual antlrcpp::Any visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx) override;
        virtual antlrcpp::Any visitConst_expression(ifccParser::Const_expressionContext *ctx) override;
        virtual antlrcpp::Any visitVar_expression(ifccParser::Var_expressionContext *ctx) override;

    private:
        std::map<std::string, SymbolTableVisitor::VariableInfo> symbolTable;
};

